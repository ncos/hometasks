/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <nori/bsdf.h>
#include <nori/frame.h>
#include <nori/warp.h>

NORI_NAMESPACE_BEGIN

class Microfacet : public BSDF {
public:
    Microfacet(const PropertyList &propList) {
        /* RMS surface roughness */
        m_alpha = propList.getFloat("alpha", 0.1f);

        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);

        /* Albedo of the diffuse base material (a.k.a "kd") */
        m_kd = propList.getColor("kd", Color3f(0.5f));

        /* To ensure energy conservation, we must scale the 
           specular component by 1-kd. 

           While that is not a particularly realistic model of what 
           happens in reality, this will greatly simplify the 
           implementation. Please see the course staff if you're 
           interested in implementing a more realistic version 
           of this BRDF. */
        m_ks = 1 - m_kd.maxCoeff();
    }


    inline float G1(const Vector3f& v, const Vector3f& h) const {
        if (v.dot(h) * Frame::cosTheta(v) <= 0)
            return 0;

        float b_div = std::abs(Frame::tanTheta(v)) * this->m_alpha;
        if (b_div == 0)
            return 1;

        float b = 1 / b_div;
        if (b >= 1.6)
            return 1;

        float b2 = b * b;
        return (3.535 * b + 2.181 * b2) / (1 + 2.276 * b + 2.577 * b2);

    }

    /// Evaluate the BRDF for the given pair of directions
    Color3f eval(const BSDFQueryRecord &bRec) const {
    	Vector3f Wi = bRec.wi;
        Vector3f Wo = bRec.wo;
        Vector3f Wh = (Wi + Wo).normalized();

        float cos_th_i = Frame::cosTheta(Wi);
        float cos_th_o = Frame::cosTheta(Wo);
        float cos_th_h = Frame::cosTheta(Wh);

        if (cos_th_i <= 0.0f || cos_th_o <= 0.0f)
            return Color3f{0.0f};

        if (Wh[0] == 0 && Wh[1] == 0 && Wh[2] == 0)
            return Color3f{0.0f};

        float D = Warp::squareToBeckmannPdf(Wh, this->m_alpha);
        float F = fresnel(Wh.dot(Wi), this->m_extIOR, this->m_intIOR);
        float G = this->G1(Wi, Wh) * this->G1(Wo, Wh);

        return this->m_kd / M_PI + this->m_ks * (D * F * G) / (4 * cos_th_i * cos_th_o * cos_th_h);
    }

    /// Evaluate the sampling density of \ref sample() wrt. solid angles
    float pdf(const BSDFQueryRecord &bRec) const {
    	if (Frame::cosTheta(bRec.wi) <= 0 || Frame::cosTheta(bRec.wo) <= 0)
            return 0;

        Vector3f Wh = (bRec.wi + bRec.wo).normalized();
        float D = Warp::squareToBeckmannPdf(Wh, this->m_alpha);
        return this->m_ks * D / (4 * Wh.dot(bRec.wo)) +
               (1 - this->m_ks) * Warp::squareToCosineHemispherePdf(bRec.wo);
    }

    /// Sample the BRDF
    Color3f sample(BSDFQueryRecord &bRec, const Point2f &_sample) const {
    	Point2f sample_{_sample};

        if (_sample.x() < this->m_ks) {
            sample_.x() = _sample.x() / this->m_ks;
            Vector3f Wh = Warp::squareToBeckmann(sample_, this->m_alpha);
            bRec.wo = 2 * bRec.wi.dot(Wh) * Wh - bRec.wi;
        }
        else {
            sample_.x() = (_sample.x() - this->m_ks) / (1 - this->m_ks);
            bRec.wo = Warp::squareToCosineHemisphere(sample_);
        }
        bRec.measure = ESolidAngle;

        float pdf_ = this->pdf(bRec);
        if (pdf_ > 0)
            return this->eval(bRec) * Frame::cosTheta(bRec.wo) / pdf_;
        
        return Color3f{0.0f};
    }

    bool isDiffuse() const {
        /* While microfacet BRDFs are not perfectly diffuse, they can be
           handled by sampling techniques for diffuse/non-specular materials,
           hence we return true here */
        return true;
    }

    std::string toString() const {
        return tfm::format(
            "Microfacet[\n"
            "  alpha = %f,\n"
            "  intIOR = %f,\n"
            "  extIOR = %f,\n"
            "  kd = %s,\n"
            "  ks = %f\n"
            "]",
            m_alpha,
            m_intIOR,
            m_extIOR,
            m_kd.toString(),
            m_ks
        );
    }
private:
    float m_alpha;
    float m_intIOR, m_extIOR;
    float m_ks;
    Color3f m_kd;
};

NORI_REGISTER_CLASS(Microfacet, "microfacet");
NORI_NAMESPACE_END

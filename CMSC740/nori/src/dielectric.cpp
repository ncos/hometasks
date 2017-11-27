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

NORI_NAMESPACE_BEGIN

/// Ideal dielectric BSDF
class Dielectric : public BSDF {
public:
    Dielectric(const PropertyList &propList) {
        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);
    }

    Color3f eval(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return Color3f(0.0f);
    }

    float pdf(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return 0.0f;
    }

    float snell(float sin_2, float n1, float n2) const {
        return n1 / n2 * sin_2;
    }

    float fresnel(float cos_2, float n1, float n2, float& cos_1) const {
        float sin_2 = std::sqrt(std::max(0.0, 1.0 - cos_2 * cos_2));
        float sin_1 = this->snell(sin_2, n1, n2);

        if (sin_1 >= 1.0)
            return 1.0;

        cos_1 = std::sqrt(1.0f - sin_1 * sin_1);

        float Rs = (n1 * cos_1 - n2 * cos_2) / (n1 * cos_1 + n2 * cos_2);
        float Rp = (n1 * cos_2 - n2 * cos_1) / (n1 * cos_2 + n2 * cos_1);
        return (Rs * Rs + Rp * Rp) / 2.0;
    }

    Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
        float cos_2 = bRec.wi.z();
        float n1 = this->m_extIOR, n2 = this->m_intIOR;
        bRec.measure = EDiscrete;

        bool negative = (cos_2 < 0);
        if (negative) {
            std::swap(n1, n2);
            cos_2 = -cos_2;
        }

        float eta = n2 / n1;

        float cos_1;
        if (sample.x() > fresnel(cos_2, n1, n2, cos_1)) {
            if (!negative) cos_1 = -cos_1;
            bRec.wo = Vector3f(-bRec.wi.x() / eta, -bRec.wi.y() / eta, cos_1).normalized();
            bRec.eta = eta;
            return Color3f(1.0 / (eta * eta));
        }

        // Reflection
        bRec.wo = Vector3f(-bRec.wi.x(), -bRec.wi.y(), bRec.wi.z());
        bRec.eta = 1.0;
        return Color3f(1.0);
    }

    std::string toString() const {
        return tfm::format(
            "Dielectric[\n"
            "  intIOR = %f,\n"
            "  extIOR = %f\n"
            "]",
            m_intIOR, m_extIOR);
    }
private:
    float m_intIOR, m_extIOR;
};

NORI_REGISTER_CLASS(Dielectric, "dielectric");
NORI_NAMESPACE_END

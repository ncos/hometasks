#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/camera.h>
#include <nori/block.h>
#include <nori/sampler.h>
#include <nori/warp.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>



extern nori::ImageBlock *globalLightBlock;

NORI_NAMESPACE_BEGIN

struct Vertex {
    Intersection its;
    Vector3f wi, wo;
    Color3f cumulative;
    bool from_spec;
    bool specular;
    float pdfFwd, pdfRev;
    Vertex (Intersection &its_) : its(its_), from_spec(false), specular(false) {}
    Vertex (Intersection &its_, const Vector3f &wi_, const Vector3f &wo_, const Color3f &clr_) 
        : its(its_), wi(wi_), wo(wo_), cumulative(clr_), from_spec(false), specular(false) {}
};


class BDPTIntegrator : public Integrator {
protected:
    bool naive;

public:
    BDPTIntegrator (const PropertyList &props) {
        this->naive = false;    
    }

    virtual void preprocess(const Scene *scene) { 
        this->camera = scene->getCamera();
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        Ray3f ray_{ray}; // Eye ray
        Color3f ret{0.0f};

        // Initialize emitters
        std::vector<const Mesh*> emitters;
        for (auto &m : scene->getMeshes())
            if (m->isEmitter()) emitters.push_back(m);

        DiscretePDF emitter_pdf;
        for (auto &e : emitters)
            emitter_pdf.append(1);    
        emitter_pdf.normalize();

        // Light ray
        int idx = emitter_pdf.sample(sampler->next1D());
        const Mesh* emitter = emitters[idx];

        // Sample a point on the emitter
        Point3f p;
        Normal3f n;
        emitter->samplePosition(sampler->next2D(), p, n);

        Intersection lits;
        if (!scene->rayIntersect(Ray3f{p + n * 0.01, -n}, lits) || (lits.p - p).dot(lits.p - p) > 0.01) {
            std::cout << "ERROR! points do not match...\n"
                      << lits.p << " " << p << "\n" << std::flush;
        }

        // Sample a ray
        BSDFQueryRecord bsdf_record{Normal3f{0, 0, 1}}; // wi does not matter for diffuse light
        emitter->getBSDF()->sample(bsdf_record, sampler->next2D());
        float Le_pdf = emitter->PDF() * emitter_pdf[idx];
        Color3f Le = emitter->getEmitter()->eval(bsdf_record.wo, bsdf_record.wo);// / (emitter->PDF() * emitter_pdf[idx]);

        std::vector<Vertex> lightPath, eyePath;

        float light_pdf = emitter->getBSDF()->pdf(bsdf_record);
        lightPath.push_back(Vertex(lits, bsdf_record.wi, bsdf_record.wo, Frame::cosTheta(bsdf_record.wo) / light_pdf)); // Manually put the first vertex
        this->genPath(scene, sampler, Ray3f{p, lits.shFrame.toWorld(bsdf_record.wo)}, lightPath, false);
        this->genPath(scene, sampler, ray, eyePath, true);

        //std::cout << "E/L sizes: " << eyePath.size() << " " << lightPath.size() << "\n";
       
        int s_max = 6;
        int t_max = 1;

        s_max = eyePath.size() - 1;
        t_max = lightPath.size() - 1;

        s_max = std::min(s_max, (int)eyePath.size() - 1);
        t_max = std::min(t_max, (int)lightPath.size() - 1);

        // Cases I and III
        for (int t = 0; t < t_max + 1; ++t) {
            Color3f ret_III = Le; 
            Vertex &lv = lightPath[t];
            Ray3f newRay{lv.its.p, (ray.o - lv.its.p).normalized()};

            // Visibility test
            Intersection v_its;
            if (scene->rayIntersect(newRay, v_its))
                continue;
            
            Point2f pixelSample = this->camera->pixelFromVisiblePoint(newRay, lv.its.p);

            float lengthSquared = (ray.o - lv.its.p).dot(ray.o - lv.its.p);
            if (lengthSquared < 0.05) continue;

            Vector3f local_wo = lv.its.shFrame.toLocal(newRay.d);

            Color3f lv_bsdf = Color3f(0.0f);
            if (t > 0) {
                ret_III *= lightPath[t - 1].cumulative;

                BSDFQueryRecord lv_bsdf_record{lv.wi, local_wo, ESolidAngle};
                float pdf_ = lv.its.mesh->getBSDF()->pdf(lv_bsdf_record);
                if (pdf_ > 0)
                    lv_bsdf = lv.its.mesh->getBSDF()->eval(lv_bsdf_record);
            } else {
                lv_bsdf = lv.cumulative;
            }

            if (!lv_bsdf.isValid()) continue;

            float dot2 = std::max(lv.its.shFrame.cosTheta(local_wo), 0.0f);
            float geo = dot2 / lengthSquared;

            ret_III *= lv_bsdf * geo;
            ret_III *= this->alpha_w(eyePath, lightPath, 1, t + 1);

            //globalLightBlock->put_special(pixelSample, ret_III / sampler->getSampleCount());
        }

        // Cases II and IV
        for (int s = 0; s < s_max + 1; ++s) {

            // Case II
            Color3f ret_II{0.0f};
            const Vertex &ev = eyePath[s];
            Color3f decay{1.0f};
            if (s > 0)
                decay = eyePath[s - 1].cumulative;

            if (ev.from_spec && ev.its.mesh->isEmitter()) {
                ret_II += decay * ev.its.mesh->getEmitter()->eval(ev.its, ev.its.shFrame.toWorld(ev.wo));// * this->alpha_w(eyePath, lightPath, s + 2, 0);
            }

            if (ev.its.mesh->getBSDF()->isDiffuse()) {
                idx = emitter_pdf.sample(sampler->next1D());
                emitter = emitters[idx];
                emitter->samplePosition(sampler->next2D(), p, n);

                // Directions
                Vector3f l2px = (p - ev.its.p).normalized();

                // Emitter is seen?
                Intersection em_its;
                if ((scene->rayIntersect(Ray3f{ev.its.p, l2px}, em_its)) && (em_its.mesh == emitter)) {
                    Vector3f l2px_local = ev.its.shFrame.toLocal(l2px);

                    BSDFQueryRecord bsdf_record{ev.wo, l2px_local, ESolidAngle};
                    Color3f bsdf = ev.its.mesh->getBSDF()->eval(bsdf_record);
                    Color3f radiance = emitter->getEmitter()->eval(em_its, -l2px);

                    // Geometric term
                    float dot1 = std::max((-l2px_local).dot(ev.its.shFrame.toLocal(n)), 0.0f);
                    float dot2 = std::max(ev.its.shFrame.cosTheta(l2px_local), 0.0f);
                    float geo = dot1 * dot2 / ((p - ev.its.p).dot(p - ev.its.p));
                    float p_brdf = ev.its.mesh->getBSDF()->pdf(bsdf_record);

                    if (dot1 > 0.0f && geo > 0) {
                        float p_light = emitter->PDF() * ((p - ev.its.p).dot(p - ev.its.p)) / dot1;
                        float w_light = p_light / (p_light + p_brdf);
                        ret_II += decay * radiance * bsdf * Color3f(geo / emitter->PDF() * w_light) / emitter_pdf[idx];
                    }
                }
                
                BSDFQueryRecord bsdf_record{ev.wo};
                const Color3f c = ev.its.mesh->getBSDF()->sample(bsdf_record, sampler->next2D());
                const Ray3f newRay{ev.its.p, ev.its.shFrame.toWorld(bsdf_record.wo)};

                Intersection its_;
                if (scene->rayIntersect(newRay, its_) && its_.mesh == emitter) {
                    Vector3f l2px = (its_.p - ev.its.p).normalized();
                    Vector3f l2px_local = its_.shFrame.toLocal(l2px);

                    float dot2 = std::max(its_.shFrame.cosTheta(-l2px_local), 0.0f);      
                    Color3f radiance = emitter->getEmitter()->eval(its_, -l2px);
            
                    if (dot2 > 0.0f) {
                        float p_brdf = ev.its.mesh->getBSDF()->pdf(bsdf_record);
                        float p_light = emitter->PDF() * ((its_.p - ev.its.p).dot(its_.p - ev.its.p)) / dot2;
                        float w_brdf = p_brdf / (p_light + p_brdf);
                        ret_II += decay * radiance * c * w_brdf / emitter_pdf[idx];
                    }
                }
            }
            // End of case II
            //ret += ret_II * (this->alpha_w(eyePath, lightPath, s + 2, 0) + this->alpha_w(eyePath, lightPath, s + 2, 1));

            // Case IV
            Color3f ret_IV{0.0f};
            for (int t = 1; t < t_max + 1; ++t) {
                Vector3f l2px = (lightPath[t].its.p - ev.its.p).normalized();

                Intersection v_its;
                if (!(scene->rayIntersect(Ray3f{ev.its.p, l2px}, v_its)) 
                    || (v_its.mesh != lightPath[t].its.mesh)) continue;

                if (s == 0 && t == 2)
                //ret_IV += Le / Le_pdf * this->mergePaths(eyePath, lightPath, s, t) * 
                //               this->alpha_w(eyePath, lightPath, s + 2, t + 1);

                ret_IV += Le * this->mergePaths(eyePath, lightPath, s, t);
            }
            ret += ret_IV;
        }

        /* 
        float g_p = 0;
        for (int s = -2; s < (int)eyePath.size(); ++s) {
            float g_line = 0;
            for (int t = -1; t < (int)lightPath.size(); ++t) {
                g_line += this->alpha_w(eyePath, lightPath, s + 2, t + 1);
                std::cout << this->alpha_w(eyePath, lightPath, s + 2, t + 1) << "\t";
            }
            std::cout << " (" << g_line << ")\n" << std::flush;
            g_p += g_line;
        }
        std::cout << "Total g_p: " << g_p << "\n-------------------\n\n";
        */
        

        Point2f pixelSample = this->camera->pixelFromVisiblePoint(ray_, its.p);
        globalLightBlock->put_special(pixelSample, ret / sampler->getSampleCount());

        return ret;
    }

    void genPath(const Scene *scene, Sampler *sampler, const Ray3f &ray_, 
                 std::vector<Vertex> &vertices, bool isEye) const {

        Ray3f ray{ray_};
        Color3f ret{1.0f};
        bool from_spec = true;

        if (vertices.size() > 0) {
            ret = vertices[vertices.size() - 1].cumulative;
            from_spec = !vertices[vertices.size() - 1].its.mesh->getBSDF()->isDiffuse();
        }

        while (true) {
            Intersection its;
            if (!scene->rayIntersect(ray, its))
                break;
            
            if (from_spec && its.mesh->isEmitter()) {
                //ret *= its.mesh->getEmitter()->eval(its, -ray.d);
                Vertex v(its);
            
                if (isEye) {
                    v.wo = its.shFrame.toLocal(-ray.d);
                } else {
                    v.wi = its.shFrame.toLocal(-ray.d);
                }

                v.cumulative = Color3f{0, 0, 0};
                v.from_spec = true;
                vertices.push_back(v);
                break;
            }

            Vertex v(its);
            BSDFQueryRecord bsdf_record{v.its.shFrame.toLocal(-ray.d)};
            ret *= v.its.mesh->getBSDF()->sample(bsdf_record, sampler->next2D());  

            v.pdfFwd = v.its.mesh->getBSDF()->pdf(bsdf_record);
            BSDFQueryRecord bsdf_record_rev{bsdf_record.wo, bsdf_record.wi, ESolidAngle};
            float pdfRev = v.its.mesh->getBSDF()->pdf(bsdf_record_rev);

            if (!v.its.mesh->getBSDF()->isDiffuse()) {
                pdfRev = 0;
                v.pdfFwd = 0;
                v.specular = true;
            }

            if (isEye) {
                v.wo = bsdf_record.wi;
                v.wi = bsdf_record.wo;
            } else {
                v.wo = bsdf_record.wo;
                v.wi = bsdf_record.wi;
            }

            ray = Ray3f{v.its.p, v.its.shFrame.toWorld(bsdf_record.wo)};
            v.cumulative = ret;

            if (ret.isZero())
                break;

            if (sampler->next1D() > 0.95)
                break;

            v.cumulative /= 0.95;


            if (vertices.size() > 0)
                vertices[vertices.size() - 1].pdfRev = pdfRev;

            v.from_spec = from_spec;
            vertices.push_back(v);
            
            from_spec = !its.mesh->getBSDF()->isDiffuse();
        }
    }

    Color3f mergePaths(std::vector<Vertex> &eyePath, std::vector<Vertex> &lightPath,
                    int s, int t) const {

        const Vertex &ev = eyePath[s];
        const Vertex &lv = lightPath[t];

        Color3f ret{1.0f};
        if (s > 0)
            ret *= eyePath[s - 1].cumulative;
        if (t > 0)
            ret *= lightPath[t - 1].cumulative;

        // Geometric term
        Vector3f etl = lv.its.p - ev.its.p;
        float lengthSquared = etl.dot(etl);
        if (lengthSquared < 0.05)
            return Color3f(0.0f);

        etl = etl.normalized();

        float dot1 = std::max(ev.its.shFrame.cosTheta(ev.its.shFrame.toLocal( etl)), 0.0f);
        float dot2 = std::max(lv.its.shFrame.cosTheta(lv.its.shFrame.toLocal(-etl)), 0.0f);
        float geo = dot1 * dot2 / lengthSquared;

        BSDFQueryRecord ev_bsdf_record{ev.wi, ev.its.shFrame.toLocal(etl), ESolidAngle};
        Color3f ev_bsdf = ev.its.mesh->getBSDF()->eval(ev_bsdf_record);

        BSDFQueryRecord lv_bsdf_record{lv.its.shFrame.toLocal(-etl), lv.wo, ESolidAngle};
        Color3f lv_bsdf = lv.its.mesh->getBSDF()->eval(lv_bsdf_record);

        return ret * ev_bsdf * lv_bsdf * Color3f(geo);
    }

    float alpha_w(std::vector<Vertex> &eyePath, std::vector<Vertex> &lightPath, int s, int t) const {
        if (this->naive) return this->alpha_w_naive(s, t);
        return this->alpha_w_mis(eyePath, lightPath, s, t);
    }

    float alpha_w_naive(int s, int t) const {
        return 1.0 / float(s + t + 1);
    }

    float alpha_w_mis(std::vector<Vertex> &eyePath, std::vector<Vertex> &lightPath, int s, int t) const {
        float sumRi = 0;
        float ri = 1;
        for (int i = s - 1; i > 0; --i) {
            float pdfRev = (eyePath[i].pdfRev == 0) ? 1 : eyePath[i].pdfRev;
            float pdfFwd = (eyePath[i].pdfFwd == 0) ? 1 : eyePath[i].pdfFwd;

            ri *= pdfRev / pdfFwd;
        
            if (eyePath[i].specular || eyePath[i - 1].specular) 
                continue;
                
            sumRi += ri;
        }

        ri = 1;
        for (int i = t - 1; i >= 0; --i) {
            float pdfRev = (lightPath[i].pdfRev == 0) ? 1 : lightPath[i].pdfRev;
            float pdfFwd = (lightPath[i].pdfFwd == 0) ? 1 : lightPath[i].pdfFwd;

            ri *= pdfRev / pdfFwd;
        
            if (lightPath[i].specular)
                continue;

            if (i > 0 && lightPath[i - 1].specular)
                continue;
                
            sumRi += ri;
        }

        //std::cout << sumRi << "\n";
        return 1.0 / float(sumRi + 1);
    }

    std::string toString() const {
        return "BDPTIntegrator[]";
    }

protected:
    const Camera *camera;

};

NORI_REGISTER_CLASS(BDPTIntegrator, "bdpt");
NORI_NAMESPACE_END

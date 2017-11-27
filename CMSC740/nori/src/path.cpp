#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/warp.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>


NORI_NAMESPACE_BEGIN


class PathIntegrator : public Integrator {
public:
    PathIntegrator(const PropertyList &props) {}

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        Ray3f ray_{ray};
        Color3f decay{1.0f};
        Color3f ret{0.0f};

        // Initialize emitters
        std::vector<const Mesh*> emitters;
        for (auto &m : scene->getMeshes())
            if (m->isEmitter()) emitters.push_back(m);

        DiscretePDF emitter_pdf;
        for (auto &e : emitters)
            emitter_pdf.append(1);    
        emitter_pdf.normalize();

        bool from_spec = true;
        for (int i = 0;; ++i) {
            if (from_spec && its.mesh->isEmitter())
                ret += its.mesh->getEmitter()->eval(its, -ray_.d) * decay;

            // Diffuse
            if (its.mesh->getBSDF()->isDiffuse()) {
                int idx = emitter_pdf.sample(sampler->next1D());
                const Mesh* emitter = emitters[idx];

                Point3f p;
                Normal3f n;
                emitter->samplePosition(sampler->next2D(), p, n);

                // Directions
                Vector3f l2px = (p - its.p).normalized();

                // Emitter is seen?
                Intersection em_its;
                if ((scene->rayIntersect(Ray3f{its.p, l2px}, em_its)) && (em_its.mesh == emitter)) {
                    Vector3f l2px_local = its.shFrame.toLocal(l2px);

                    BSDFQueryRecord bsdf_record{its.shFrame.toLocal(-ray_.d), l2px_local, ESolidAngle};
                    Color3f bsdf = its.mesh->getBSDF()->eval(bsdf_record);
                    Color3f radiance = emitter->getEmitter()->eval(em_its, -l2px);

                    // Geometric term
                    float dot1 = std::max((-l2px_local).dot(its.shFrame.toLocal(n)), 0.0f);
                    float dot2 = std::max(its.shFrame.cosTheta(l2px_local), 0.0f);
                    float geo = dot1 * dot2 / ((p - its.p).dot(p - its.p));
            
                    if (dot1 > 0.0f) {
                        float p_brdf = its.mesh->getBSDF()->pdf(bsdf_record);
                        float p_light = emitter->PDF() * ((p - its.p).dot(p - its.p)) / dot1;
                        float w_light = p_light / (p_light + p_brdf);
                        ret += radiance * bsdf * Color3f(geo / emitter->PDF() * w_light) / emitter_pdf[idx] * decay;
                    }
                }

                BSDFQueryRecord bsdf_record{its.shFrame.toLocal(-ray_.d)};
                const Color3f c = its.mesh->getBSDF()->sample(bsdf_record, sampler->next2D());
                const Ray3f newRay{its.p, its.shFrame.toWorld(bsdf_record.wo)};

                Intersection its_;
                if (scene->rayIntersect(newRay, its_) && its_.mesh == emitter) {
                    Vector3f l2px = (its_.p - its.p).normalized();
                    Vector3f l2px_local = its_.shFrame.toLocal(l2px);

                    float dot2 = std::max(its_.shFrame.cosTheta(-l2px_local), 0.0f);      
                    Color3f radiance = emitter->getEmitter()->eval(its_, -l2px);
            
                    if (dot2 > 0.0f) {
                        float p_brdf = its.mesh->getBSDF()->pdf(bsdf_record);
                        float p_light = emitter->PDF() * ((its_.p - its.p).dot(its_.p - its.p)) / dot2;
                        float w_brdf = p_brdf / (p_light + p_brdf);
                        ret += radiance * c * w_brdf / emitter_pdf[idx] * decay;
                    }
                }
            }

            // Shoot new ray and start over
            from_spec = !its.mesh->getBSDF()->isDiffuse();
            BSDFQueryRecord bsdfRecord{its.shFrame.toLocal(-ray_.d)};
            const Color3f c = its.mesh->getBSDF()->sample(bsdfRecord, sampler->next2D());
            
            decay *= c;
            ray_ = Ray3f{its.p, its.shFrame.toWorld(bsdfRecord.wo)};
            if (!scene->rayIntersect(ray_, its))
                break;

            if (i > 5 && sampler->next1D() > 0.95)
                break;

            decay /= 0.95;
        }

        return ret;
    }

    std::string toString() const {
        return "PathIntegrator[]";
    }
};

NORI_REGISTER_CLASS(PathIntegrator, "path");
NORI_NAMESPACE_END

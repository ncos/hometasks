#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/warp.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>


NORI_NAMESPACE_BEGIN


class SimplePathIntegrator : public Integrator {
public:
    SimplePathIntegrator(const PropertyList &props) {}

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);
        
        if (its.mesh->isEmitter())
            return its.mesh->getEmitter()->eval(its, -ray.d);
       
        if (!its.mesh->getBSDF()->isDiffuse()) {
            if (sampler->next1D() > 0.95) return Color3f(0.0f);
            
            BSDFQueryRecord bsdfRecord{its.shFrame.toLocal(-ray.d)};
            const Color3f c = its.mesh->getBSDF()->sample(bsdfRecord, sampler->next2D());
            const Ray3f newRay{its.p, its.shFrame.toWorld(bsdfRecord.wo)};
            return c * this->Li(scene, sampler, newRay) / 0.95;            
        }

        Color3f ret{0.0f};

        // Diffuse
        std::vector<const Mesh*> emitters;
        for (auto &m : scene->getMeshes())
            if (m->isEmitter()) emitters.push_back(m);

        DiscretePDF emitter_pdf;
        for (auto &e : emitters)
            emitter_pdf.append(1);    
        emitter_pdf.normalize();

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

            BSDFQueryRecord bsdf_record{its.shFrame.toLocal(-ray.d), l2px_local, ESolidAngle};
            Color3f bsdf = its.mesh->getBSDF()->eval(bsdf_record);
            Color3f radiance = emitter->getEmitter()->eval(em_its, -l2px);

            // Geometric term
            float dot1 = std::max((-l2px_local).dot(its.shFrame.toLocal(n)), 0.0f);
            float dot2 = std::max(its.shFrame.cosTheta(l2px_local), 0.0f);
            float geo = dot1 * dot2 / ((p - its.p).dot(p - its.p));

            ret = radiance * bsdf * Color3f(geo / (emitter->PDF() * emitter_pdf[idx]));
        }
        
        if (sampler->next1D() > 0.95) return ret;

        BSDFQueryRecord bsdfRecord{its.shFrame.toLocal(-ray.d)};
        const Color3f c = its.mesh->getBSDF()->sample(bsdfRecord, sampler->next2D());
        const Ray3f newRay{its.p, its.shFrame.toWorld(bsdfRecord.wo)};

        Intersection its_;
        if (scene->rayIntersect(newRay, its_) && !its_.mesh->isEmitter())
            return ret + c * Li(scene, sampler, newRay) / 0.95;
  
        return ret;
    }

    std::string toString() const {
        return "SimplePathIntegrator[]";
    }
};

NORI_REGISTER_CLASS(SimplePathIntegrator, "path_simple");
NORI_NAMESPACE_END

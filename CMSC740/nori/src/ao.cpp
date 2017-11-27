#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/warp.h>


NORI_NAMESPACE_BEGIN


class AOIntegrator : public Integrator {
public:
    AOIntegrator(const PropertyList &props) {}

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        Vector3f light_ray = Warp::squareToCosineHemisphere(sampler->next2D());
        float sample_pdf = Warp::squareToCosineHemispherePdf(light_ray);
        if (sample_pdf <= 0.0f) return Color3f(0.0f);
        
        float cos_th = std::max(its.shFrame.cosTheta(light_ray), 0.0f);

        Ray3f shadow_ray{its.p, its.shFrame.toWorld(light_ray)};
        if (scene->rayIntersect(shadow_ray)) return Color3f(0.0f);
        
        return (cos_th / M_PI) / sample_pdf;
    }

    std::string toString() const {
        return "AOIntegrator[]";
    }
};


NORI_REGISTER_CLASS(AOIntegrator, "ao");
NORI_NAMESPACE_END

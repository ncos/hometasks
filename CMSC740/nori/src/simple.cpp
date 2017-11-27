#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/warp.h>


NORI_NAMESPACE_BEGIN


class SimpleIntegrator : public Integrator {
public:
    SimpleIntegrator(const PropertyList &props) {
        this->lpos = props.getPoint("position");
        this->lpower = props.getColor("energy");
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
        Intersection its;
        if (!scene->rayIntersect(ray, its))
            return Color3f(0.0f);

        Vector3f light_ray = this->lpos - its.p;

        Ray3f shadow_ray{its.p, light_ray};
        if (scene->rayIntersect(shadow_ray))
            return Color3f(0.0f);

        float cos_th = std::max(its.shFrame.cosTheta(its.shFrame.toLocal(light_ray.normalized())), 0.0f);
        return this->lpower / (4.0f * M_PI * M_PI * light_ray.dot(light_ray)) * cos_th;
    }

    std::string toString() const {
        return "SimpleIntegrator[]";
    }

protected:
    Point3f lpos;
    Color3f lpower;
};


NORI_REGISTER_CLASS(SimpleIntegrator, "simple");
NORI_NAMESPACE_END

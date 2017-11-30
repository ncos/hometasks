#include <nori/emitter.h>


NORI_NAMESPACE_BEGIN


class AreaLight : public Emitter {
public:
    AreaLight(const PropertyList &props) {
        this->m_radiance = props.getColor("radiance");
    }

    Color3f eval(const Intersection &its, const Vector3f &d) const {
        if (its.shFrame.n.dot(d) <= 0) return Color3f(0.0f);
        return this->m_radiance;
    }

    Color3f eval(const Normal3f &n, const Vector3f &d) const {
        if (n.dot(d) <= 0) return Color3f(0.0f);
        return this->m_radiance;
    }

    std::string toString() const {
        return "AreaLight[]";
    }

private:
    Color3f m_radiance;
};


NORI_REGISTER_CLASS(AreaLight, "area");
NORI_NAMESPACE_END

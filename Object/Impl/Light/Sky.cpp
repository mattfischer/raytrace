#include "Object/Impl/Light/Sky.hpp"

#include "Object/Scene.hpp"
#include "Object/Intersection.hpp"

namespace Object::Impl::Light {
    Sky::Sky(const Math::Radiance &radiance)
    : mRadiance(radiance)
    {
    }

    Object::Light::Sample Sky::sample(Math::Sampler &sampler, const Math::Point &pnt) const
    {
        return {Math::Radiance(), pnt, Math::Vector(), Math::Pdf(), 0.0f};
    }

    Math::Radiance Sky::radiance(const Object::Intersection &isect) const
    {
        return mRadiance;
    }

    Math::Radiance Sky::radiance(const Math::Vector &direction) const
    {
        return mRadiance;
    }

    Math::Pdf Sky::pdf(const Object::Intersection &isect) const
    {
        return 0.0f;
    }

    bool Sky::testVisible(const Object::Scene &scene, const Sample &sample) const
    {
        Math::Ray ray(sample.origin, sample.direction);
        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
        Object::Intersection isect = scene.intersect(beam, FLT_MAX, false);

        return !isect.valid();
    }
}
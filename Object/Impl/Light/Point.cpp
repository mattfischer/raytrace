#include "Object/Impl/Light/Point.hpp"

#include "Object/Scene.hpp"

namespace Object::Impl::Light {
    Point::Point(const Math::Point &position, const Math::Radiance &radiance)
    : mPosition(position), mRadiance(radiance)
    {
    }

    Object::Light::Sample Point::sample(Math::Sampler &sampler, const Math::Point &pnt) const
    {
        Math::Vector dirIn = mPosition - pnt;
        float d = dirIn.magnitude();
        dirIn = dirIn / d;

        Math::Pdf pdfAngular(d * d, true);

        return {mRadiance, pnt, dirIn, pdfAngular, d};
    }

    bool Point::testVisible(const Object::Scene &scene, const Sample &sample) const
    {
        Math::Ray ray(sample.origin, sample.direction);
        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
        Object::Intersection isect = scene.intersect(beam, sample.distance, false);

        return !isect.valid();
    }

    Math::Radiance Point::radiance(const Object::Intersection &isect) const
    {
        return Math::Radiance();
    }

    Math::Pdf Point::pdf(const Object::Intersection &isect) const
    {
        return 0.0f;
    }

    void Point::writeProxy(PointLightProxy &proxy) const
    {
        mPosition.writeProxy(proxy.position);
        mRadiance.writeProxy(proxy.radiance);
    }
}
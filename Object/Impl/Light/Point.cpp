#include "Object/Impl/Light/Point.hpp"

#include "Object/Scene.hpp"

namespace Object::Impl::Light {
    Point::Point(const Math::Point &position, const Math::Radiance &radiance)
    : mPosition(position), mRadiance(radiance)
    {
    }

    std::tuple<Math::Radiance, Math::Vector, Math::Pdf> Point::sample(Math::Sampler &sampler, const Math::Point &pnt) const
    {
        Math::Vector dirIn = mPosition - pnt;
        float d = dirIn.magnitude();
        dirIn = dirIn / d;

        Math::Pdf pdfAngular(d * d, true);

        return std::make_tuple(mRadiance, dirIn, pdfAngular);
    }

    bool Point::testVisible(const Object::Scene &scene, const Math::Point &pnt, const Math::Vector &dirIn) const
    {
        float d = (mPosition - pnt).magnitude();

        Math::Ray ray(pnt, dirIn);
        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
        Object::Intersection isect = scene.intersect(beam, d, false);

        return !isect.valid();
    }

    void Point::writeProxy(PointLightProxy &proxy) const
    {
        mPosition.writeProxy(proxy.position);
        mRadiance.writeProxy(proxy.radiance);
    }
}
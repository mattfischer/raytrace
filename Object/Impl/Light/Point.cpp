#include "Object/Impl/Light/Point.hpp"

namespace Object::Impl::Light {
    Point::Point(const Math::Point &position, const Math::Radiance &radiance)
    : mPosition(position), mRadiance(radiance)
    {
    }

    Math::Radiance Point::sample(Math::Sampler &sampler, const Math::Point &pnt, Math::Point &pntSample, Math::Pdf &pdf) const
    {
        pntSample = mPosition;
        Math::Vector dirOut = pnt - pntSample;
        float d = dirOut.magnitude();

        pdf = Math::Pdf(d * d, true);
        return mRadiance;
    }

    bool Point::didIntersect(const Object::Intersection &isect) const
    {
        return false;
    }

    void Point::writeProxy(PointLightProxy &proxy) const
    {
        mPosition.writeProxy(proxy.position);
        mRadiance.writeProxy(proxy.radiance);
    }
}
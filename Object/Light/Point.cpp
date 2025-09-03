#include "Object/Light/Point.hpp"

namespace Object::Light {
    Point::Point(const Math::Point &position, const Math::Radiance &radiance)
    : mPosition(position), mRadiance(radiance)
    {
    }

    Math::Radiance Point::sample(Math::Sampler::Base &sampler, const Math::Point &pnt, Math::Point &pntSample, float &dotSample, float &pdf, bool &pdfDelta) const
    {
        pntSample = mPosition;
        dotSample = 0.0f;
        pdf = 1.0f;
        pdfDelta = true;
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
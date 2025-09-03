#ifndef OBJECT_LIGHT_POINT_HPP
#define OBJECT_LIGHT_POINT_HPP

#include "Object/Light/Base.hpp"

#include "Object/CLProxies.hpp"

namespace Object::Light {
    class Point : public Base
    {
    public:
        Point(const Math::Point &position, const Math::Radiance &radiance);

        virtual Math::Radiance sample(Math::Sampler::Base &sampler, const Math::Point &pnt, Math::Point &pntSample, float &dotSample, float &pdf, bool &pdfDelta) const override;
        virtual bool didIntersect(const Object::Intersection &isect) const override;

        void writeProxy(PointLightProxy &proxy) const;

    private:
        Math::Point mPosition;
        Math::Radiance mRadiance;
    };
}
#endif
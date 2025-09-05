#ifndef OBJECT_IMPL_LIGHT_POINT_HPP
#define OBJECT_IMPL_LIGHT_POINT_HPP

#include "Object/Light.hpp"

#include "Object/CLProxies.hpp"

namespace Object::Impl::Light {
    class Point : public Object::Light
    {
    public:
        Point(const Math::Point &position, const Math::Radiance &radiance);

        virtual Math::Radiance sample(Math::Sampler &sampler, const Math::Point &pnt, Math::Point &pntSample, Math::Pdf &pdf) const override;
        virtual bool didIntersect(const Object::Intersection &isect) const override;

        void writeProxy(PointLightProxy &proxy) const;

    private:
        Math::Point mPosition;
        Math::Radiance mRadiance;
    };
}
#endif
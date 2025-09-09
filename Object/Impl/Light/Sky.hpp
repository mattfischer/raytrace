#ifndef OBJECT_IMPL_LIGHT_SKY_HPP
#define OBJECT_IMPL_LIGHT_SKY_HPP

#include "Object/Light.hpp"

namespace Object::Impl::Light {
    class Sky : public Object::Light {
    public:
        Sky(const Math::Radiance &radiance);

        virtual Sample sample(Math::Sampler &sampler, const Math::Point &pnt) const override;
        virtual Math::Radiance radiance(const Object::Intersection &isect) const override;
        virtual Math::Radiance radiance(const Math::Vector &direction) const override;
        virtual Math::Pdf pdf(const Object::Intersection &isect) const override;

        virtual bool testVisible(const Object::Scene &scene, const Sample &sample) const override;

    private:
        Math::Radiance mRadiance;
    };
}
#endif
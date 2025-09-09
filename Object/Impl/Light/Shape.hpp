#ifndef OBJECT_IMPL_LIGHT_SHAPE_HPP
#define OBJECT_IMPL_LIGHT_SHAPE_HPP

#include "Object/Light.hpp"
#include "Object/Shape.hpp"

namespace Object::Impl::Light {
    class Shape : public Object::Light
    {
    public:
        Shape(const Object::Shape &shape, const Math::Radiance &radiance);

        virtual Sample sample(Math::Sampler &sampler, const Math::Point &pnt) const override;
        virtual Math::Radiance radiance(const Object::Intersection &isect) const override;
        virtual Math::Pdf pdf(const Object::Intersection &isect) const override;

        virtual bool testVisible(const Object::Scene &scene, const Sample &sample) const override;

    private:
        const Object::Shape &mShape;
        Math::Radiance mRadiance;
    };
}

#endif
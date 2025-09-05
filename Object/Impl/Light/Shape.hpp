#ifndef OBJECT_IMPL_LIGHT_SHAPE_HPP
#define OBJECT_IMPL_LIGHT_SHAPE_HPP

#include "Object/Light.hpp"
#include "Object/Shape.hpp"

namespace Object::Impl::Light {
    class Shape : public Object::Light
    {
    public:
        Shape(const Object::Shape &shape, const Math::Radiance &radiance);

        virtual Math::Radiance sample(Math::Sampler &sampler, const Math::Point &pnt, Math::Vector &dirIn, Math::Pdf &pdf) const override;
        virtual bool testVisible(const Object::Scene &scene, const Math::Point &pnt, const Math::Vector &dirIn) const override;

    private:
        const Object::Shape &mShape;
        Math::Radiance mRadiance;
    };
}

#endif
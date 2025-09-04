#ifndef OBJECT_IMPL_LIGHT_SHAPE_HPP
#define OBJECT_IMPL_LIGHT_SHAPE_HPP

#include "Object/Light.hpp"
#include "Object/Shape.hpp"

namespace Object::Impl::Light {
    class Shape : public Object::Light
    {
    public:
        Shape(const Object::Shape &shape, const Math::Radiance &radiance);

        virtual Math::Radiance sample(Math::Sampler &sampler, const Math::Point &pnt, Math::Point &pntSample, float &dotSample, float &pdf, bool &pdfDelta) const override;
        virtual bool didIntersect(const Object::Intersection &isect) const override;

    private:
        const Object::Shape &mShape;
        Math::Radiance mRadiance;
    };
}

#endif
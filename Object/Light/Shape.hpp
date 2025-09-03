#ifndef OBJECT_LIGHT_SHAPE_HPP
#define OBJECT_LIGHT_SHAPE_HPP

#include "Object/Light/Base.hpp"
#include "Object/Shape/Base.hpp"

namespace Object::Light {
    class Shape : public Base
    {
    public:
        Shape(const Object::Shape::Base &shape, const Math::Radiance &radiance);

        virtual Math::Radiance sample(Math::Sampler::Base &sampler, const Math::Point &pnt, Math::Point &pntSample, float &dotSample, float &pdf, bool &pdfDelta) const override;
        virtual bool didIntersect(const Object::Intersection &isect) const override;

    private:
        const Object::Shape::Base &mShape;
        Math::Radiance mRadiance;
    };
}

#endif
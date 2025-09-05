#include "Object/Impl/Light/Shape.hpp"

namespace Object::Impl::Light {
    Shape::Shape(const Object::Shape &shape, const Math::Radiance &radiance)
    : mShape(shape), mRadiance(radiance)
    {
    }

    Math::Radiance Shape::sample(Math::Sampler &sampler, const Math::Point &pnt, Math::Point &pntSample, float &dotSample, Math::Pdf &pdf) const
    {
        Math::Radiance rad;
        Math::Normal nrmSample;
        if(mShape.sample(sampler, pntSample, nrmSample, pdf)) {
            Math::Vector dirOut = pnt - pntSample;
            float d = dirOut.magnitude();
            dirOut = dirOut / d;
            dotSample = std::abs(dirOut * nrmSample);

            rad = mRadiance * dotSample;
        }

        return rad;
    }

    bool Shape::didIntersect(const Object::Intersection &isect) const
    {
        return &(isect.primitive().shape()) == &mShape;
    }

}
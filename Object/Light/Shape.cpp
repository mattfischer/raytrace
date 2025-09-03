#include "Object/Light/Shape.hpp"

namespace Object::Light {
    Shape::Shape(const Object::Shape::Base &shape, const Math::Radiance &radiance)
    : mShape(shape), mRadiance(radiance)
    {
    }

    Math::Radiance Shape::sample(Math::Sampler::Base &sampler, const Math::Point &pnt, Math::Point &pntSample, float &dotSample, float &pdf, bool &pdfDelta) const
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
        pdfDelta = false;

        return rad;
    }

    bool Shape::didIntersect(const Object::Intersection &isect) const
    {
        return &(isect.primitive().shape()) == &mShape;
    }

}
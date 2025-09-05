#include "Object/Impl/Light/Shape.hpp"

namespace Object::Impl::Light {
    Shape::Shape(const Object::Shape &shape, const Math::Radiance &radiance)
    : mShape(shape), mRadiance(radiance)
    {
    }

    Math::Radiance Shape::sample(Math::Sampler &sampler, const Math::Point &pnt, Math::Point &pntSample, Math::Pdf &pdf) const
    {
        Math::Radiance rad;
        Math::Normal nrmSample;
        Math::Pdf pdfArea;
        if(mShape.sample(sampler, pntSample, nrmSample, pdfArea)) {
            Math::Vector dirOut = pntSample - pnt;
            float d = dirOut.magnitude();
            dirOut = dirOut / d;
            float dot = std::abs(dirOut * nrmSample);
            pdf = pdfArea * d * d / dot;

            rad = mRadiance * dot;
        }

        return rad;
    }

    bool Shape::didIntersect(const Object::Intersection &isect) const
    {
        return &(isect.primitive().shape()) == &mShape;
    }

}
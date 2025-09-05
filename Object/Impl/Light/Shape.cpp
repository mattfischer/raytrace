#include "Object/Impl/Light/Shape.hpp"

#include "Object/Scene.hpp"

namespace Object::Impl::Light {
    Shape::Shape(const Object::Shape &shape, const Math::Radiance &radiance)
    : mShape(shape), mRadiance(radiance)
    {
    }

    Math::Radiance Shape::sample(Math::Sampler &sampler, const Math::Point &pnt, Math::Vector &dirIn, Math::Pdf &pdf) const
    {
        Math::Radiance rad;
        Math::Point pntSample;
        Math::Normal nrmSample;
        Math::Pdf pdfArea;
        if(mShape.sample(sampler, pntSample, nrmSample, pdfArea)) {
            dirIn = pntSample - pnt;
            float d = dirIn.magnitude();
            dirIn = dirIn / d;
            float dot = std::abs(dirIn * nrmSample);
            pdf = pdfArea * d * d / dot;

            rad = mRadiance * dot;
        }

        return rad;
    }

    bool Shape::testVisible(const Object::Scene &scene, const Math::Point &pnt, const Math::Vector &dirIn) const
    {
        Math::Ray ray(pnt, dirIn);
        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
        Object::Intersection isect = scene.intersect(beam, FLT_MAX, true);

        return (isect.valid() && &(isect.primitive().shape()) == &mShape);
    }
}
#include "Object/Impl/Light/Shape.hpp"

#include "Object/Scene.hpp"

namespace Object::Impl::Light {
    Shape::Shape(const Object::Shape &shape, const Math::Radiance &radiance)
    : mShape(shape), mRadiance(radiance)
    {
    }

    std::tuple<Math::Radiance, Math::Vector, Math::Pdf> Shape::sample(Math::Sampler &sampler, const Math::Point &pnt) const
    {
        Math::Radiance rad;
        Math::Vector dirIn;
        Math::Pdf pdfAngular;

        auto [pntSample, nrmSample, pdfArea] = mShape.sample(sampler);
        if(pdfArea > 0.0f) {
            dirIn = pntSample - pnt;
            float d = dirIn.magnitude();
            dirIn = dirIn / d;
            float dot = std::abs(dirIn * nrmSample);
            pdfAngular = pdfArea * d * d / dot;

            rad = mRadiance * dot;
        }

        return std::make_tuple(rad, dirIn, pdfAngular);
    }

    bool Shape::testVisible(const Object::Scene &scene, const Math::Point &pnt, const Math::Vector &dirIn) const
    {
        Math::Ray ray(pnt, dirIn);
        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
        Object::Intersection isect = scene.intersect(beam, FLT_MAX, true);

        return (isect.valid() && &(isect.primitive().shape()) == &mShape);
    }
}
#ifndef OBJECT_LIGHT_HPP
#define OBJECT_LIGHT_HPP

#include "Math/Sampler.hpp"
#include "Math/Pdf.hpp"
#include "Math/Point.hpp"
#include "Math/Radiance.hpp"
#include "Math/Vector.hpp"

#include <tuple>

namespace Object {
    class Scene;
    class Intersection;

    class Light {
    public:
        virtual std::tuple<Math::Radiance, Math::Vector, Math::Pdf> sample(Math::Sampler &sampler, const Math::Point &pnt) const = 0;
        virtual Math::Pdf pdf(const Object::Intersection &isect) const = 0;
        virtual Math::Radiance radiance(const Object::Intersection &isect) const = 0;

        virtual bool testVisible(const Object::Scene &scene, const Math::Point &pnt, const Math::Vector &dirIn) const = 0;
    };
}

#endif
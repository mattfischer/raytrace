#ifndef OBJECT_LIGHT_HPP
#define OBJECT_LIGHT_HPP

#include "Math/Sampler.hpp"
#include "Math/Pdf.hpp"
#include "Math/Point.hpp"
#include "Math/Radiance.hpp"
#include "Math/Vector.hpp"

namespace Object {
    class Scene;

    class Light {
    public:
        virtual Math::Radiance sample(Math::Sampler &sampler, const Math::Point &pnt, Math::Vector &dirIn, Math::Pdf &pdf) const = 0;
    
        virtual bool testVisible(const Object::Scene &scene, const Math::Point &pnt, const Math::Vector &dirIn) const = 0;
    };
}

#endif
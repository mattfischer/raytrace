#ifndef OBJECT_LIGHT_HPP
#define OBJECT_LIGHT_HPP

#include "Math/Sampler.hpp"
#include "Math/Pdf.hpp"
#include "Math/Point.hpp"
#include "Math/Radiance.hpp"

#include "Object/Intersection.hpp"

namespace Object {
    class Light {
    public:
        virtual Math::Radiance sample(Math::Sampler &sampler, const Math::Point &pnt, Math::Point &pntSample, float &dotSample, Math::Pdf &pdf) const = 0;
    
        virtual bool didIntersect(const Object::Intersection &isect) const = 0;
    };
}

#endif
#ifndef OBJECT_LIGHT_BASE_HPP
#define OBJECT_LIGHT_BASE_HPP

#include "Math/Sampler/Base.hpp"
#include "Math/Point.hpp"
#include "Math/Radiance.hpp"

#include "Object/Intersection.hpp"

namespace Object::Light {
    class Base {
    public:
        virtual Math::Radiance sample(Math::Sampler::Base &sampler, const Math::Point &pnt, Math::Point &pntSample, float &dotSample, float &pdf, bool &pdfDelta) const = 0;
    
        virtual bool didIntersect(const Object::Intersection &isect) const = 0;
    };
}

#endif
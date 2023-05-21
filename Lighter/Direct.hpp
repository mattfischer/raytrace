#ifndef LIGHTER_DIRECT_HPP
#define LIGHTER_DIRECT_HPP

#include "Lighter/Base.hpp"

namespace Lighter {
    class Direct : public Base
    {
    public:
        virtual Object::Radiance light(const Object::Intersection &isect, Render::Sampler &sampler) const;
    };
}

#endif
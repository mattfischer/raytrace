#ifndef LIGHTER_DIRECT_HPP
#define LIGHTER_DIRECT_HPP

#include "Lighter/Base.hpp"

namespace Lighter {
    class Direct : public Base
    {
    public:
        Object::Radiance light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const override;
    };
}

#endif
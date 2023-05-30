#ifndef RENDER_LIGHTER_DIRECT_HPP
#define RENDER_LIGHTER_DIRECT_HPP

#include "Render/Lighter/Base.hpp"

namespace Render {
    namespace Lighter {
        class Direct : public Base
        {
        public:
            Object::Radiance light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const override;
        };
    }
}

#endif
#ifndef RENDER_CPU_LIGHTER_DIRECT_HPP
#define RENDER_CPU_LIGHTER_DIRECT_HPP

#include "Render/Cpu/Lighter/Base.hpp"

namespace Render {
    namespace Cpu {
        namespace Lighter {
            class Direct : public Base
            {
            public:
                Math::Radiance light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const override;
            };
        }
    }
}

#endif
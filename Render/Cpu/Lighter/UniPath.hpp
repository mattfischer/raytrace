#ifndef RENDER_CPU_LIGHTER_UNIPATH_HPP
#define RENDER_CPU_LIGHTER_UNIPATH_HPP

#include "Render/Cpu/Lighter/Base.hpp"

namespace Render {
    namespace Cpu {
        namespace Lighter
        {
            class UniPath : public Base
            {
            public:
                Object::Radiance light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const override;
            };
        }
    }
}

#endif

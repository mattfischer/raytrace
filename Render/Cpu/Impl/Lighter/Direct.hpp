#ifndef RENDER_CPU_IMPL_LIGHTER_DIRECT_HPP
#define RENDER_CPU_IMPL_LIGHTER_DIRECT_HPP

#include "Render/Cpu/Lighter.hpp"

namespace Render::Cpu::Impl::Lighter {
    class Direct : public Render::Cpu::Lighter
    {
    public:
        Math::Radiance light(const Object::Intersection &isect, Math::Sampler &sampler) const override;
    };
}

#endif
#ifndef RENDER_CPU_LIGHTER_HPP
#define RENDER_CPU_LIGHTER_HPP

#include "Render/Cpu/Executor.hpp"
#include "Render/Framebuffer.hpp"

#include "Math/Vector.hpp"

#include "Math/Radiance.hpp"
#include "Object/Intersection.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <vector>
#include <memory>

namespace Render::Cpu {
    class Lighter {
    public:
        Lighter();
        virtual ~Lighter();

        virtual Math::Radiance light(const Object::Intersection &isect, Math::Sampler &sampler) const = 0;
        virtual std::vector<std::unique_ptr<Render::Cpu::Executor::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);
    };
}

#endif

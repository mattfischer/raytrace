#ifndef RENDER_CPU_LIGHTER_BASE_HPP
#define RENDER_CPU_LIGHTER_BASE_HPP

#include "Render/Executor.hpp"
#include "Render/Framebuffer.hpp"

#include "Math/Vector.hpp"

#include "Object/Radiance.hpp"
#include "Object/Intersection.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <vector>
#include <memory>

namespace Render {
    namespace Cpu {
        namespace Lighter {
            class Base {
            public:
                Base();
                virtual ~Base();

                virtual Object::Radiance light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const = 0;
                virtual std::vector<std::unique_ptr<Render::Executor::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);
            };
        }
    }
}

#endif

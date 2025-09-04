#ifndef RENDER_CPU_IMPL_LIGHTER_IRRADIANCECACHED_HPP
#define RENDER_CPU_IMPL_LIGHTER_IRRADIANCECACHED_HPP

#include "Render/Cpu/Lighter.hpp"

#include "Render/Cpu/Impl/Lighter/Direct.hpp"
#include "Render/Cpu/Impl/Lighter/UniPath.hpp"

#include <memory>

namespace Render::Cpu::Impl::Lighter {
    class IrradianceCached : public Render::Cpu::Lighter
    {
    public:
        struct Settings {
            unsigned int indirectSamples;
            float cacheThreshold;
        };

        IrradianceCached(const Settings &settings);

        Math::Radiance light(const Object::Intersection &isect, Math::Sampler &sampler) const override;

        std::vector<std::unique_ptr<Render::Cpu::Executor::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer) override;

    private:
        void prerenderPixel(unsigned int x, unsigned int y, Render::Framebuffer &framebuffer, const Object::Scene &scene, Math::Sampler &sampler);

        class Cache;

        std::unique_ptr<Impl::Lighter::UniPath> mUniPathLighter;
        std::unique_ptr<Impl::Lighter::Direct> mDirectLighter;
        std::unique_ptr<Cache> mCache;
        Settings mSettings;
    };
}

#endif

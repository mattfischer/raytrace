#ifndef LIGHTER_IRRADIANCECACHED_HPP
#define LIGHTER_IRRADIANCECACHED_HPP

#include "Lighter/Base.hpp"

#include "Lighter/Direct.hpp"
#include "Lighter/UniPath.hpp"

#include <memory>

namespace Lighter {
    class IrradianceCached : public Base
    {
    public:
        struct Settings {
            unsigned int indirectSamples;
            float cacheThreshold;
        };

        IrradianceCached(const Settings &settings);

        Object::Radiance light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const override;

        std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer) override;

    private:
        void prerenderPixel(unsigned int x, unsigned int y, Render::Framebuffer &framebuffer, const Object::Scene &scene, Math::Sampler::Base &sampler);

        class Cache;

        std::unique_ptr<Lighter::UniPath> mUniPathLighter;
        std::unique_ptr<Lighter::Direct> mDirectLighter;
        std::unique_ptr<Cache> mCache;
        Settings mSettings;
    };
}

#endif

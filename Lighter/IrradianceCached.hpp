#ifndef LIGHTER_IRRADIANCECACHED_HPP
#define LIGHTER_IRRADIANCECACHED_HPP

#include "Lighter/Base.hpp"

#include "Lighter/Direct.hpp"
#include "Lighter/UniPath.hpp"
#include "Lighter/IrradianceCache.hpp"

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

        virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler) const;

        virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);

    private:
        void prerenderPixel(unsigned int x, unsigned int y, Render::Framebuffer &framebuffer, const Object::Scene &scene, Render::Sampler &sampler);

        std::unique_ptr<Lighter::UniPath> mUniPathLighter;
        std::unique_ptr<Lighter::Direct> mDirectLighter;
        IrradianceCache mIrradianceCache;
        Settings mSettings;
    };
}

#endif

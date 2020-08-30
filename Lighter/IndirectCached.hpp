#ifndef LIGHTER_INDIRECTCACHED_HPP
#define LIGHTER_INDIRECTCACHED_HPP

#include "Lighter/Base.hpp"

#include "Lighter/IrradianceCache.hpp"

namespace Lighter {
    class IndirectCached : public Base
    {
    public:
        IndirectCached(std::unique_ptr<Lighter::Base> lighter, int indirectSamples, float cacheThreshold);

        virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;

        virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);

    private:
        void prerenderPixel(int x, int y, Render::Framebuffer &framebuffer, const Object::Scene &scene, Render::Sampler &sampler);

        std::unique_ptr<Lighter::Base> mLighter;
        IrradianceCache mIrradianceCache;
        int mIndirectSamples;
    };
}

#endif

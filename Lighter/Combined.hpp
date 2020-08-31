#ifndef LIGHTER_COMBINED_HPP
#define LIGHTER_COMBINED_HPP

#include "Lighter/Base.hpp"
#include "Lighter/IndirectCached.hpp"

namespace Lighter {
    class Combined : public Base
    {
    public:
        struct Settings {
            bool radiantLighting;
            bool specularLighting;
            int specularMaxGeneration;
            bool directLighting;
            bool indirectLighting;
            int indirectSamples;
            bool irradianceCaching;
            float irradianceCacheThreshold;
        };

        Combined(Settings &settings);

        virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;

        virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);

    private:
        Object::Radiance lightDiffuseIndirect(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;
        Object::Radiance lightDirect(const Object::Intersection &intersection, Render::Sampler &sampler, int generation, bool misSpecular) const;
        Object::Radiance lightRadiant(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;
        Object::Radiance lightSpecular(const Object::Intersection &intersection, Render::Sampler &sampler, int generation, bool misDirect) const;
        Object::Radiance lightTransmit(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;

        Settings mSettings;
        std::unique_ptr<Lighter::IndirectCached> mIndirectCachedLighter;
    };
}
#endif

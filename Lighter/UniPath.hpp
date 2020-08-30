#ifndef LIGHTER_UNIPATH_HPP
#define LIGHTER_UNIPATH_HPP

#include "Lighter/Base.hpp"
#include "Lighter/IrradianceCache.hpp"

#include "Math/OrthonormalBasis.hpp"

namespace Lighter {
    class UniPath : public Base
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

        UniPath(Settings &settings);

        virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;

        virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);

    private:
        Object::Radiance lightDiffuseIndirect(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;
        Object::Radiance lightDirect(const Object::Intersection &intersection, Render::Sampler &sampler, int generation, bool misSpecular) const;
        Object::Radiance lightRadiant(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;
        Object::Radiance lightSpecular(const Object::Intersection &intersection, Render::Sampler &sampler, int generation, bool misDirect) const;
        Object::Radiance lightTransmit(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;

        void prerenderPixel(int x, int y, Render::Framebuffer &framebuffer, const Object::Scene &scene, Render::Sampler &sampler);
        Object::Radiance sampleIrradiance(const Object::Intersection &intersection, const Math::OrthonormalBasis &basis, Render::Sampler &sampler, Math::Vector &localIncidentDirection) const;

        Settings mSettings;
        IrradianceCache mIrradianceCache;
    };
}
#endif

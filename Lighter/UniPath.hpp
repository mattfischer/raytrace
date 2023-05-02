#ifndef LIGHTER_UNIPATH_HPP
#define LIGHTER_UNIPATH_HPP

#include "Lighter/Base.hpp"
#include "Lighter/IndirectCached.hpp"

#include "Object/PointLight.hpp"

namespace Lighter
{
    class UniPath : public Base
    {
    public:
        struct Settings {
            bool irradianceCaching;
            unsigned int indirectSamples;
            float irradianceCacheThreshold;
        };

        UniPath(const Settings &settings);

        virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler) const;
        virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);

    private:
        Object::Radiance lightInternal(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;
        Object::Radiance lightRadiant(const Object::Intersection &intersection) const;
        Object::Radiance lightTransmitted(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;
        Object::Radiance lightReflected(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;

        std::unique_ptr<Lighter::IndirectCached> mIndirectCachedLighter;
    };
}
#endif // UNIPATH_HPP

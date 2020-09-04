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
            int indirectSamples;
            float irradianceCacheThreshold;
        };

        UniPath(const Settings &settings);

        virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;
        virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);

    private:
        Object::Radiance lightRadiant(const Object::Intersection &intersection) const;
        Object::Radiance lightTransmitted(const Object::Intersection &intersection, Render::Sampler &sampler) const;
        Object::Radiance lightReflected(const Object::Intersection &intersection, Render::Sampler &sampler) const;

        Object::Radiance sampleLight(const Object::Intersection &intersection, const Object::Primitive &light, Render::Sampler &sampler, Math::Vector &incidentDirection, float &pdfAngular) const;
        Object::Radiance evaluatePointLight(const Object::Intersection &intersection, const Object::PointLight &pointLight, Math::Vector &incidentDirection) const;
        Object::Radiance sampleBrdf(const Object::Intersection &intersection, const Object::Brdf::Base &brdf, Render::Sampler &sampler, Math::Vector &incidentDirection, float &lightPdf) const;

        std::unique_ptr<Lighter::IndirectCached> mIndirectCachedLighter;
    };
}
#endif // UNIPATH_HPP

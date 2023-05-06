#define _USE_MATH_DEFINES
#include "Lighter/UniPath.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <algorithm>

namespace Lighter
{
    UniPath::UniPath(const Settings &settings)
    {
        if(settings.irradianceCaching) {
            Settings subSettings;
            subSettings.irradianceCaching = false;
            std::unique_ptr<UniPath> subLighter = std::make_unique<UniPath>(subSettings);
            mIndirectCachedLighter = std::make_unique<Lighter::IndirectCached>(std::move(subLighter),settings.indirectSamples, settings.irradianceCacheThreshold);
        }
    }

    Object::Radiance UniPath::light(const Object::Intersection &intersection, Render::Sampler &sampler) const
    {
        return lightInternal(intersection, sampler, 0);
    }

    Object::Radiance UniPath::lightInternal(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        Object::Radiance emittedRadiance = lightRadiant(intersection);
        Object::Radiance transmittedRadiance = lightTransmitted(intersection, sampler, generation);
        Object::Radiance reflectedRadiance = lightReflected(intersection, sampler, generation);
        
        return emittedRadiance + transmittedRadiance + reflectedRadiance;
    }

    std::vector<std::unique_ptr<Render::Job>> UniPath::createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer)
    {
        std::vector<std::unique_ptr<Render::Job>> jobs;

        if(mIndirectCachedLighter) {
            jobs = mIndirectCachedLighter->createPrerenderJobs(scene, framebuffer);
        }

        return jobs;
    }

    Object::Radiance UniPath::lightRadiant(const Object::Intersection &intersection) const
    {
        return intersection.primitive().surface().radiance();
    }

    Object::Radiance UniPath::lightTransmitted(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        const Object::Scene &scene = intersection.scene();
        const Object::Surface &surface = intersection.primitive().surface();
        const Math::Normal &facingNormal = surface.facingNormal(intersection);
        Math::Vector outgoingDirection = -intersection.ray().direction();

        Object::Radiance radiance;
        float threshold = 0.5f;

        if (!surface.opaque()) {
            float roulette = sampler.getValue();
            if(roulette > threshold) {
                return radiance;
            }

            Math::Point offsetPoint = intersection.point() - Math::Vector(facingNormal) * 0.01f;

            Math::Vector incidentDirection;
            float pdf;
            Object::Color transmitted = surface.sampleTransmitted(intersection, sampler, incidentDirection, pdf);
            Math::Ray transmitRay(offsetPoint, incidentDirection);
            Math::Beam beam(transmitRay, Math::Bivector(), Math::Bivector());
            Object::Intersection intersection2 = scene.intersect(beam);

            Math::Normal incidentNormal = -facingNormal;
            float dot = incidentDirection * incidentNormal;
            if (intersection2.valid()) {
                Object::Radiance irradiance = lightInternal(intersection2, sampler, generation + 1) * dot;
                Object::Radiance transmittedRadiance = irradiance * transmitted;
                radiance += transmittedRadiance;
            }
        }

        return radiance / threshold;
    }

    Object::Radiance UniPath::lightReflected(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        const Object::Surface &surface = intersection.primitive().surface();
        const Object::Scene &scene = intersection.scene();
        const Math::Normal &normal = surface.facingNormal(intersection);
        const Math::Vector outgoingDirection = -intersection.ray().direction();
        
        Object::Radiance radiance;

        Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01f;

        Math::Vector incidentDirection;
        float pdf;
        Object::Color reflected = surface.sample(intersection, sampler, incidentDirection, pdf);
        float dot = incidentDirection * normal;
        
        if(dot > 0) {            
            Object::Color throughput = reflected * dot / pdf;

            float threshold = 0.0f;
            float roulette = sampler.getValue();
            if(generation == 0) {
                threshold = 1.0f;
            } else if(generation < 10) {
                threshold = std::min(1.0f, throughput.maximum());
            }

            if(roulette < threshold) {
                Math::Ray reflectRay(offsetPoint, incidentDirection);
                Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
                Object::Intersection intersection2 = scene.intersect(beam);

                if (intersection2.valid()) {
                    Object::Radiance radiance2 = lightInternal(intersection2, sampler, generation + 1);
                    float misWeight = 1.0f;
                    if(intersection2.primitive().surface().radiance().magnitude() > 0) {
                        float sampleDot = -intersection2.primitive().surface().facingNormal(intersection2) * incidentDirection;
                        float pdfArea = pdf * sampleDot / (intersection2.distance() * intersection2.distance());
                        float pdfLight = 0.0f;
                        const Object::Shape::Base::Sampler *shapeSampler = intersection2.primitive().shape().sampler();
                        if (shapeSampler) {
                            pdfLight = 1.0f / shapeSampler->surfaceArea();
                        }
                        misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
                    }
                    Object::Radiance sampleRadiance = radiance2 * throughput;
                    if(mIndirectCachedLighter) {
                        Object::Radiance indirectIrradiance = (radiance2 - intersection2.primitive().surface().radiance()) * dot;
                        Object::Radiance indirectRadiance = indirectIrradiance * surface.lambert() * surface.albedo(intersection) / (float)M_PI;
                        sampleRadiance = (sampleRadiance - indirectRadiance).clamp();
                        sampleRadiance += mIndirectCachedLighter->light(intersection, sampler);
                    }
                    radiance += sampleRadiance * misWeight / threshold;
                }
            }
        }

        for (const Object::Primitive &light : intersection.scene().areaLights()) {
            const Object::Radiance &objectRadiance = light.surface().radiance();
            const Object::Shape::Base::Sampler *shapeSampler = light.shape().sampler();

            if (!shapeSampler) {
                continue;
            }

            Math::Point2D surfacePoint = sampler.getValue2D();

            Math::Point samplePoint;
            Math::Normal sampleNormal;

            shapeSampler->sample(surfacePoint, samplePoint, sampleNormal);

            Math::Vector incidentDirection = samplePoint - offsetPoint;
            float distance = incidentDirection.magnitude();
            incidentDirection = incidentDirection / distance;
            float pdf = 1.0f / shapeSampler->surfaceArea();
            float sampleDot = std::abs(incidentDirection * sampleNormal);

            float dot = incidentDirection * normal;
            if(dot > 0) {
                Math::Ray ray(offsetPoint, incidentDirection);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection intersection2 = scene.intersect(beam);

                if (intersection2.valid() && &(intersection2.primitive()) == &light) {
                    Object::Radiance irradiance = objectRadiance * sampleDot * dot / (distance * distance);
                    Object::Radiance sampleRadiance = irradiance * surface.reflected(intersection, incidentDirection);
                    float pdfBrdf = surface.pdf(intersection, incidentDirection) * sampleDot / (intersection2.distance() * intersection2.distance());
                    float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                    radiance += sampleRadiance * misWeight / pdf;
                }
            }
        }

        for (const std::unique_ptr<Object::PointLight> &pointLight : intersection.scene().pointLights()) {
            Math::Vector incidentDirection = pointLight->position() - offsetPoint;
            float distance = incidentDirection.magnitude();
            incidentDirection = incidentDirection / distance;

            Math::Ray ray(offsetPoint, incidentDirection);
            Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
            Object::Intersection intersection2 = scene.intersect(beam);

            if (!intersection2.valid() || intersection2.distance() >= distance) {
                float dot = incidentDirection * normal;
                Object::Radiance irradiance = pointLight->radiance() * dot / (distance * distance);
                radiance += irradiance * surface.reflected(intersection, incidentDirection);
            }
        }

        return radiance;
    }
}

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
        Object::Radiance emittedRadiance = lightRadiant(intersection);
        Object::Radiance transmittedRadiance = lightTransmitted(intersection, sampler);
        Object::Radiance reflectedRadiance = lightReflected(intersection, sampler);

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

    Object::Radiance UniPath::lightTransmitted(const Object::Intersection &intersection, Render::Sampler &sampler) const
    {
        const Object::Scene &scene = intersection.scene();
        const Object::Surface &surface = intersection.primitive().surface();
        const Math::Ray &ray = intersection.ray();
        const Math::Normal &normal = intersection.facingNormal();
        const Object::Color &albedo = intersection.albedo();
        Math::Vector outgoingDirection = -ray.direction();
        bool reverse = (intersection.normal() * outgoingDirection < 0);

        Object::Radiance radiance;
        float threshold = 0.5f;

        if (surface.brdf().hasTransmit()) {
            float roulette = sampler.getValue();
            if(roulette > threshold) {
                return radiance;
            }

            Math::Point offsetPoint = intersection.point() - Math::Vector(normal) * 0.01;

            float ratio = 1.0f / surface.brdf().transmitIor();
            if (reverse) {
                ratio = 1.0f / ratio;
            }

            float c1 = outgoingDirection * normal;
            float c2 = std::sqrt(1.0f - ratio * ratio * (1.0f - c1 * c1));

            Math::Vector incidentDirection = Math::Vector(normal) * (ratio * c1 - c2) - outgoingDirection * ratio;
            Math::Ray transmitRay(offsetPoint, incidentDirection);
            Math::Beam beam(transmitRay, Math::Bivector(), Math::Bivector());
            Object::Intersection intersection2 = scene.intersect(beam);

            Math::Normal incidentNormal = -normal;
            float dot = incidentDirection * incidentNormal;
            if (intersection2.valid()) {
                Object::Radiance irradiance = light(intersection2, sampler) * dot;
                Object::Radiance transmittedRadiance = surface.brdf().transmitted(irradiance, incidentDirection, incidentNormal, albedo);
                radiance += transmittedRadiance / (outgoingDirection * normal);
            }
        }

        return radiance / threshold;
    }

    Object::Radiance UniPath::lightReflected(const Object::Intersection &intersection, Render::Sampler &sampler) const
    {
        const Math::Normal &normal = intersection.facingNormal();
        const Math::Vector outgoingDirection = -intersection.ray().direction();
        const Object::Surface &surface = intersection.primitive().surface();
        Object::Radiance radiance;

        Math::Vector incidentDirection;
        float lightPdf;
        Object::Radiance sampledRadiance = sampleBrdf(intersection, sampler, incidentDirection, lightPdf);
        float pdf = surface.brdf().pdf(incidentDirection, normal, outgoingDirection);
        float weight = pdf / (pdf + lightPdf);
        radiance += sampledRadiance * weight;

        for (const Object::Primitive &light : intersection.scene().areaLights()) {
            Math::Vector incidentDirection;
            float pdf;
            Object::Radiance sampledRadiance = sampleLight(intersection, light, sampler, incidentDirection, pdf);
            if(incidentDirection * normal > 0) {
                float brdfPdf = surface.brdf().pdf(incidentDirection, normal, outgoingDirection);
                float weight = pdf / (pdf + brdfPdf);
                radiance += sampledRadiance * weight;
            }
        }

        for (const std::unique_ptr<Object::PointLight> &pointLight : intersection.scene().pointLights()) {
            Math::Vector incidentDirection;
            radiance += evaluatePointLight(intersection, *pointLight, incidentDirection);
        }

        Object::Radiance indirectCachedRadiance;
        if(mIndirectCachedLighter) {
            radiance += mIndirectCachedLighter->light(intersection, sampler);
        }

        return radiance;
    }

    Object::Radiance UniPath::sampleLight(const Object::Intersection &intersection, const Object::Primitive &light, Render::Sampler &sampler, Math::Vector &incidentDirection, float &pdfAngular) const
    {
        Object::Radiance radiance;

        const Math::Point &point = intersection.point();
        const Math::Normal &normal = intersection.facingNormal();
        const Object::Scene &scene = intersection.scene();
        const Object::Brdf::Base &brdf = intersection.primitive().surface().brdf();
        const Math::Vector &outgoingDirection = -intersection.ray().direction();
        const Object::Color &albedo = intersection.albedo();

        const Object::Radiance &objectRadiance = light.surface().radiance();
        const Object::Shape::Base::Sampler *shapeSampler = light.shape().sampler();

        pdfAngular = 0;
        if (!shapeSampler) {
            return radiance;
        }

        float surfaceArea = shapeSampler->surfaceArea();
        Math::Point2D surfacePoint = sampler.getValue2D();

        Math::Point samplePoint;
        Math::Normal sampleNormal;

        shapeSampler->sample(surfacePoint, samplePoint, sampleNormal);

        incidentDirection = samplePoint - point;
        float distance = incidentDirection.magnitude();
        incidentDirection = incidentDirection / distance;
        float pdfArea = 1.0f / surfaceArea;
        float sampleDot = std::abs(incidentDirection * sampleNormal);

        float dot = incidentDirection * normal;
        if(dot > 0) {
            Math::Point offsetPoint = point + Math::Vector(normal) * 0.0001f;
            Math::Ray ray(offsetPoint, incidentDirection);
            Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
            Object::Intersection intersection2 = scene.intersect(beam);

            if (intersection2.valid() && &(intersection2.primitive()) == &light) {
                Object::Radiance irradiance = objectRadiance * sampleDot * dot / (distance * distance);
                radiance += brdf.reflected(irradiance, incidentDirection, normal, outgoingDirection, albedo);
            }

            pdfAngular = std::min((pdfArea * distance * distance) / sampleDot, 1000.0f);
        }

        return radiance / pdfArea;
    }

    Object::Radiance UniPath::evaluatePointLight(const Object::Intersection &intersection, const Object::PointLight &pointLight, Math::Vector &incidentDirection) const
    {
        Object::Radiance radiance;

        const Math::Point &point = intersection.point();
        const Math::Normal &normal = intersection.facingNormal();
        const Object::Scene &scene = intersection.scene();
        const Object::Brdf::Base &brdf = intersection.primitive().surface().brdf();
        const Object::Color &albedo = intersection.albedo();
        const Math::Vector &outgoingDirection = -intersection.ray().direction();

        Math::Point offsetPoint = point + Math::Vector(normal) * 0.01f;
        incidentDirection = pointLight.position() - offsetPoint;
        float distance = incidentDirection.magnitude();
        incidentDirection = incidentDirection / distance;

        Math::Ray ray(offsetPoint, incidentDirection);
        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
        Object::Intersection intersection2 = scene.intersect(beam);

        if (!intersection2.valid() || intersection2.distance() >= distance) {
            float dot = incidentDirection * normal;
            Object::Radiance irradiance = pointLight.radiance() * dot / (distance * distance);
            radiance += brdf.reflected(irradiance, incidentDirection, normal, outgoingDirection, albedo);
        }

        return radiance;
    }

    Object::Radiance UniPath::sampleBrdf(const Object::Intersection &intersection, Render::Sampler &sampler, Math::Vector &incidentDirection, float &pdfAngularLight) const
    {
        const Object::Scene &scene = intersection.scene();
        const Object::Color &albedo = intersection.albedo();
        const Object::Surface &surface = intersection.primitive().surface();
        const Object::Brdf::Base &brdf = surface.brdf();
        const Math::Ray &ray = intersection.ray();
        const Math::Normal &normal = intersection.facingNormal();
        Math::Vector outgoingDirection = -ray.direction();

        Object::Radiance radiance;

        Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01f;

        incidentDirection = brdf.sample(sampler, normal, outgoingDirection);

        pdfAngularLight = 0;
        float pdfAngular = brdf.pdf(incidentDirection, normal, outgoingDirection);

        Object::Radiance testRadiance = surface.brdf().reflected(Object::Radiance(1,1,1) / std::sqrt(3.0f), incidentDirection, normal, outgoingDirection, albedo) / pdfAngular;
        float threshold = std::max(std::min(testRadiance.magnitude(), 0.9f), 0.1f);
        float roulette = sampler.getValue();
        float dot = incidentDirection * normal;
        if(roulette < threshold && dot > 0) {
            Math::Ray reflectRay(offsetPoint, incidentDirection);
            Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
            Object::Intersection intersection2 = scene.intersect(beam);

            if (intersection2.valid()) {
                Object::Radiance irradiance = light(intersection2, sampler) * dot;
                radiance = surface.brdf().reflected(irradiance, incidentDirection, normal, outgoingDirection, albedo);

                if(mIndirectCachedLighter) {
                    Object::Radiance indirectIrradiance = irradiance - intersection2.primitive().surface().radiance() * dot;
                    Object::Radiance indirectRadiance = indirectIrradiance * surface.brdf().lambert() * albedo / M_PI;
                    radiance = (radiance - indirectRadiance).clamp();
                }

                if(intersection2.primitive().surface().radiance().magnitude() > 0) {
                    const Object::Shape::Base::Sampler *shapeSampler = intersection2.primitive().shape().sampler();
                    if (shapeSampler) {
                        float pdfAreaLight = 1.0f / shapeSampler->surfaceArea();
                        float lightDot = -intersection2.facingNormal() * incidentDirection;
                        pdfAngularLight = std::min((pdfAreaLight * intersection2.distance() * intersection2.distance()) / lightDot, 1000.0f);
                    }
                }
            }
        }

        return radiance / (pdfAngular * threshold);
    }
}

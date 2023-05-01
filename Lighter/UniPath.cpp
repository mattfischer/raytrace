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
        const Math::Ray &ray = intersection.ray();
        const Math::Normal &normal = intersection.facingNormal();
        Math::Vector outgoingDirection = -ray.direction();
        bool reverse = (intersection.normal() * outgoingDirection < 0);

        Object::Radiance radiance;
        float threshold = 0.5f;

        if (!surface.brdf().opaque()) {
            float roulette = sampler.getValue();
            if(roulette > threshold) {
                return radiance;
            }

            Math::Point offsetPoint = intersection.point() - Math::Vector(normal) * 0.01f;

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
                Object::Radiance irradiance = lightInternal(intersection2, sampler, generation + 1) * dot;
                Object::Radiance transmittedRadiance = irradiance * surface.transmitted(intersection, incidentDirection);
                radiance += transmittedRadiance / (outgoingDirection * normal);
            }
        }

        return radiance / threshold;
    }

    Object::Radiance UniPath::lightReflected(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        const Object::Scene &scene = intersection.scene();
        const Math::Normal &normal = intersection.facingNormal();
        const Math::Vector outgoingDirection = -intersection.ray().direction();
        const Object::Surface &surface = intersection.primitive().surface();

        Object::Radiance radiance;

        Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01f;

        Math::Vector incidentDirection = surface.brdf().sample(sampler, normal, outgoingDirection);
        float dot = incidentDirection * normal;
        
        if(dot > 0) {
            float threshold = 0.5f;
            float roulette = 0.0f;
            if(generation > 0) {
                roulette = sampler.getValue();
            } else if(generation > 10) {
                roulette = 1.0f;
            }

            if(roulette < threshold) {
                Math::Ray reflectRay(offsetPoint, incidentDirection);
                Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
                Object::Intersection intersection2 = scene.intersect(beam);

                if (intersection2.valid() && intersection2.primitive().surface().radiance().magnitude() == 0) {
                    Object::Radiance radiance2 = lightInternal(intersection2, sampler, generation + 1);
                    Object::Radiance irradiance = radiance2 * dot;
                    Object::Radiance sampleRadiance = irradiance * surface.reflected(intersection, incidentDirection);
                    float pdf = surface.brdf().pdf(incidentDirection, normal, outgoingDirection);
                    radiance += sampleRadiance / (threshold * pdf);
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
                    
                    radiance += sampleRadiance / pdf;
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

    Object::Radiance UniPath::sampleIrradiance(const Object::Intersection &intersection, const Math::OrthonormalBasis &basis, Render::Sampler &sampler, Math::Vector &localIncidentDirection) const
    {
        const Math::Normal &normal = intersection.facingNormal();

		float phi = 2 * M_PI * sampler.getValue();
		float theta = std::asin(std::sqrt(sampler.getValue()));
		Math::Vector direction = basis.localToWorld(Math::Vector::fromPolar(phi, theta, 1));
		Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01;
		Math::Ray ray(offsetPoint, direction);
		Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
		Object::Intersection intersection2 = intersection.scene().intersect(beam);

		localIncidentDirection = Math::Vector::fromPolar(phi, theta, 1);
		Object::Radiance irradiance;
		if (intersection2.valid()) {
			irradiance = lightInternal(intersection2, sampler, 0) * std::cos(theta);
		}

		return irradiance;
    }
}

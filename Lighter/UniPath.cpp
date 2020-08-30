#define _USE_MATH_DEFINES
#include "Lighter/UniPath.hpp"

#include "Object/Scene.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <cmath>

namespace Lighter {
    UniPath::UniPath(Settings &settings)
        : mSettings(settings)
    {
        if(settings.indirectLighting && settings.irradianceCaching)
        {
            Settings subSettings;
            subSettings.directLighting = true;
            subSettings.indirectLighting = false;
            subSettings.specularLighting = false;
            subSettings.radiantLighting = false;
            std::unique_ptr<Lighter::Base> subLighter = std::make_unique<UniPath>(subSettings);
            mIndirectCachedLighter = std::make_unique<IndirectCached>(std::move(subLighter), settings.indirectSamples, settings.irradianceCacheThreshold);
        }
    }

    Object::Radiance UniPath::light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        Object::Radiance radiance;

        if (mSettings.directLighting) {
            radiance += lightDirect(intersection, sampler, generation, mSettings.specularLighting);
        }

        if (mSettings.indirectLighting) {
            radiance += lightDiffuseIndirect(intersection, sampler, generation);
        }

        if (mSettings.radiantLighting) {
            radiance += lightRadiant(intersection, sampler, generation);
        }

        if (mSettings.specularLighting) {
            radiance += lightSpecular(intersection, sampler, generation, mSettings.directLighting);
        }

        radiance += lightTransmit(intersection, sampler, generation);

        return radiance;
    }

    std::vector<std::unique_ptr<Render::Job>> UniPath::createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer)
    {
        std::vector<std::unique_ptr<Render::Job>> jobs;

        if(mIndirectCachedLighter) {
            jobs = mIndirectCachedLighter->createPrerenderJobs(scene, framebuffer);
        }

        return jobs;
    }

    Object::Radiance UniPath::lightDiffuseIndirect(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        Object::Radiance radiance;

        if (mSettings.irradianceCaching) {
            radiance = mIndirectCachedLighter->light(intersection, sampler, generation);
        }
        else {
            if (!intersection.primitive().surface().brdf().hasDiffuse()) {
                return Object::Radiance();
            }

            const Math::Point &point = intersection.point();
            const Math::Normal &normal = intersection.facingNormal();
            const Object::Color &albedo = intersection.albedo();
            const Object::Brdf::Base &brdf = intersection.primitive().surface().brdf().diffuse();

            Math::OrthonormalBasis basis(normal);

            Math::Vector incomingDirection;

            float phi = 2 * M_PI * sampler.getValue();
            float theta = std::asin(std::sqrt(sampler.getValue()));
            Math::Vector direction = basis.localToWorld(Math::Vector::fromPolar(phi, theta, 1));
            Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01;
            Math::Ray ray(offsetPoint, direction);
            Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
            Object::Intersection intersection2 = intersection.scene().intersect(beam);

            Object::Radiance irradiance;
            if (intersection2.valid()) {
                irradiance = lightDirect(intersection2, sampler, 0, false) * theta;
            }

            radiance = irradiance * albedo * brdf.lambert();
        }

        return radiance;
    }

    Object::Radiance UniPath::lightDirect(const Object::Intersection &intersection, Render::Sampler &sampler, int generation, bool misSpecular) const
    {
        bool hasDiffuse = intersection.primitive().surface().brdf().hasDiffuse();
        bool hasSpecular = intersection.primitive().surface().brdf().hasSpecular();

        if (!hasDiffuse && !hasSpecular) {
            return Object::Radiance();
        }

        const Object::Scene &scene = intersection.scene();
        const Math::Point &point = intersection.point();
        const Math::Normal &normal = intersection.facingNormal();
        const Object::Color &albedo = intersection.albedo();

        const Object::Brdf::Base &diffuseBrdf = intersection.primitive().surface().brdf().diffuse();
        const Object::Brdf::Base &specularBrdf = intersection.primitive().surface().brdf().specular();
        const Math::Vector &outgoingDirection = -intersection.ray().direction();

        Math::OrthonormalBasis basis(normal);

        Object::Radiance radiance;
        for (const Object::Primitive &light : scene.areaLights()) {
            const Object::Radiance &objectRadiance = light.surface().radiance();
            const Object::Shape::Base::Sampler *shapeSampler = light.shape().sampler();
            if (!shapeSampler) {
                continue;
            }

            float surfaceArea = shapeSampler->surfaceArea();
            Math::Point2D surfacePoint = sampler.getValue2D();

            Math::Point samplePoint;
            Math::Normal sampleNormal;

            shapeSampler->sample(surfacePoint, samplePoint, sampleNormal);

            Math::Vector incidentDirection = samplePoint - point;
            float distance = incidentDirection.magnitude();
            incidentDirection = incidentDirection / distance;

            float dot = incidentDirection * normal;
            if (dot > 0) {
                Math::Point offsetPoint = point + Math::Vector(normal) * 0.0001;
                Math::Ray ray(offsetPoint, incidentDirection);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection intersection2 = scene.intersect(beam);

                if (intersection2.valid() && &(intersection2.primitive()) == &light) {
                    float dot = incidentDirection * normal;
                    float sampleDot = abs(incidentDirection * sampleNormal);
                    float pdf = 1.0f / surfaceArea;

                    Object::Radiance incidentRadiance = objectRadiance * sampleDot * dot / (distance * distance);
                    Object::Radiance transmittedRadiance = incidentRadiance;
                    if (hasSpecular && misSpecular) {
                        Object::Radiance reflectedRadiance = specularBrdf.reflected(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / pdf;
                        float specularPdf = specularBrdf.pdf(incidentDirection, normal, outgoingDirection) * sampleDot / (distance * distance);
                        float weight = pdf / (pdf + specularPdf);
                        radiance += reflectedRadiance * weight;
                        transmittedRadiance = specularBrdf.transmitted(incidentRadiance, incidentDirection, normal, albedo);
                    }

                    if (hasDiffuse) {
                        radiance += diffuseBrdf.reflected(transmittedRadiance, incidentDirection, normal, outgoingDirection, albedo) / pdf;
                    }
                }
            }
        }

        for (const std::unique_ptr<Object::PointLight> &pointLight : scene.pointLights()) {
            Math::Point offsetPoint = point + Math::Vector(normal) * 0.01;
            Math::Vector incidentDirection = pointLight->position() - offsetPoint;
            float distance = incidentDirection.magnitude();
            incidentDirection = incidentDirection / distance;

            Math::Ray ray(offsetPoint, incidentDirection);
            Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
            Object::Intersection intersection2 = scene.intersect(beam);

            if (!intersection2.valid() || intersection2.distance() >= distance) {
                float dot = incidentDirection * normal;
                if (dot > 0) {
                    Object::Radiance incidentRadiance = pointLight->radiance() * dot / (distance * distance);
                    Object::Radiance transmittedRadiance = incidentRadiance;
                    if (hasSpecular && mSettings.specularLighting) {
                        transmittedRadiance = intersection.primitive().surface().brdf().specular().transmitted(incidentRadiance, incidentDirection, normal, albedo);
                    }

                    if (hasDiffuse) {
                        radiance += diffuseBrdf.reflected(transmittedRadiance, incidentDirection, normal, outgoingDirection, albedo);
                    }
                }
            }
        }

        return radiance;
    }


    Object::Radiance UniPath::lightRadiant(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        return intersection.primitive().surface().radiance();
    }

    Object::Radiance UniPath::lightSpecular(const Object::Intersection &intersection, Render::Sampler &sampler, int generation, bool misDirect) const
    {
        const Object::Surface &surface = intersection.primitive().surface();
        const Math::Ray &ray = intersection.ray();
        const Math::Normal &normal = intersection.facingNormal();
        Math::Vector outgoingDirection = -ray.direction();

        Object::Radiance radiance;

        if (surface.brdf().hasSpecular() && generation < mSettings.specularMaxGeneration) {
            const Object::Scene &scene = intersection.scene();
            const Object::Brdf::Base &brdf = surface.brdf().specular();
            const Object::Color &albedo = intersection.albedo();

            Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01;

            Math::Point2D samplePoint = sampler.getValue2D();

            Math::Vector incidentDirection = brdf.sample(samplePoint, normal, outgoingDirection);
            float pdf = brdf.pdf(incidentDirection, normal, outgoingDirection);
            float dot = incidentDirection * normal;
            if(dot > 0) {
                Math::Ray reflectRay(offsetPoint, incidentDirection);
                Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
                Object::Intersection intersection2 = scene.intersect(beam);

                if (intersection2.valid()) {
                    Object::Radiance incidentRadiance = light(intersection2, sampler, generation + 1) * dot;
                    Object::Radiance reflectedRadiance = brdf.reflected(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / pdf;
                    float weight = 1.0f;
                    if (misDirect && intersection2.primitive().surface().radiance().magnitude() > 0) {
                        float directPdf = (1.0f / intersection2.primitive().shape().sampler()->surfaceArea()) * intersection2.distance() * intersection2.distance() / std::abs(intersection2.normal() * outgoingDirection);
                        weight = pdf / (pdf + directPdf);
                    }
                    radiance += reflectedRadiance * weight;
                }
            }
        }

        return radiance;
    }

    Object::Radiance UniPath::lightTransmit(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        const Object::Scene &scene = intersection.scene();
        const Object::Surface &surface = intersection.primitive().surface();
        const Math::Ray &ray = intersection.ray();
        const Math::Normal &normal = intersection.facingNormal();
        const Object::Color &albedo = intersection.albedo();
        Math::Vector outgoingDirection = -ray.direction();
        bool reverse = (intersection.normal() * outgoingDirection < 0);

        Object::Radiance radiance;

        if (surface.brdf().hasTransmit() && generation < mSettings.specularMaxGeneration) {
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
                Object::Radiance incidentRadiance = light(intersection2, sampler, generation + 1) * dot;
                Object::Radiance transmittedRadiance = incidentRadiance;
                if (intersection.primitive().surface().brdf().hasSpecular()) {
                    transmittedRadiance = intersection.primitive().surface().brdf().specular().transmitted(incidentRadiance, incidentDirection, incidentNormal, albedo);
                }
                if (intersection.primitive().surface().brdf().hasDiffuse()) {
                    transmittedRadiance = intersection.primitive().surface().brdf().diffuse().transmitted(incidentRadiance, incidentDirection, incidentNormal, albedo);
                }

                radiance += transmittedRadiance / (outgoingDirection * normal);
            }
        }

        return radiance;
    }
}

#define _USE_MATH_DEFINES
#include "Lighter/UniPath.hpp"

#include "Object/Scene.hpp"
#include "Math/OrthonormalBasis.hpp"

#include "Render/TileJobSimple.hpp"

#include <cmath>

namespace Lighter {
    UniPath::UniPath(Settings &settings)
        : mSettings(settings)
        , mIrradianceCache(mSettings.irradianceCacheThreshold)
    {
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

        auto func = [&](int x, int y, Render::Framebuffer &framebuffer, Render::Sampler &sampler) {
            prerenderPixel(x, y, framebuffer, scene, sampler);
        };

        if (mSettings.irradianceCaching) {
            jobs.push_back(std::make_unique<Render::TileJobSimple>(framebuffer, std::move(func)));
        }

        return jobs;
    }

    Object::Radiance UniPath::lightDiffuseIndirect(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        if (!intersection.primitive().surface().brdf().hasDiffuse()) {
            return Object::Radiance();
        }

        const Math::Point &point = intersection.point();
        const Math::Normal &normal = intersection.facingNormal();
        const Object::Color &albedo = intersection.albedo();
        const Object::Brdf::Base &brdf = intersection.primitive().surface().brdf().diffuse();
        const Math::Vector &outgoingDirection = -intersection.ray().direction();

        Object::Radiance radiance;
        if (mSettings.irradianceCaching) {
            Object::Radiance irradiance = mIrradianceCache.interpolateUnlocked(point, normal);
            radiance = irradiance * albedo * brdf.lambert() / M_PI;
        }
        else {
            Math::OrthonormalBasis basis(intersection.facingNormal());

            Math::Vector incomingDirection;
            Object::Radiance irradiance = sampleIrradiance(intersection, basis, sampler, incomingDirection);
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
            irradiance = lightDirect(intersection2, sampler, 0, false) * theta;
        }

        return irradiance;
    }

    void UniPath::prerenderPixel(int x, int y, Render::Framebuffer &framebuffer, const Object::Scene &scene, Render::Sampler &sampler)
    {
        Object::Color pixelColor;
        sampler.startSequence();
        Math::Beam beam = scene.camera().createPixelBeam(Math::Point2D(x, y), framebuffer.width(), framebuffer.height(), Math::Point2D());

        Object::Intersection intersection = scene.intersect(beam);

        if (intersection.valid() && intersection.primitive().surface().brdf().hasDiffuse()) {
            const Math::Point &point = intersection.point();
            const Math::Normal &normal = intersection.facingNormal();

            if (!mIrradianceCache.test(point, normal)) {
                Math::OrthonormalBasis basis(normal);

                float mean = 0;
                int den = 0;
                Object::Radiance radiance;
                const int M = std::sqrt(mSettings.indirectSamples);
                const int N = mSettings.indirectSamples / M;
                std::vector<Object::Radiance> samples;
                std::vector<float> sampleDistances;
                samples.resize(M * N);
                sampleDistances.resize(M * N);
                for (int k = 0; k < N; k++) {
                    for (int j = 0; j < M; j++) {
                        sampler.startSample();

                        float phi = 2 * M_PI * (k + sampler.getValue()) / N;
                        float theta = std::asin(std::sqrt((j + sampler.getValue()) / M));
                        Math::Vector direction = basis.localToWorld(Math::Vector::fromPolar(phi, theta, 1));

                        Math::Point offsetPoint = point + Math::Vector(normal) * 0.01;
                        Math::Ray ray(offsetPoint, direction);
                        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                        Object::Intersection intersection2 = scene.intersect(beam);

                        if (intersection2.valid()) {
                            mean += 1 / intersection2.distance();
                            den++;
                            Object::Radiance incidentRadiance = lightDirect(intersection2, sampler, 1, false);

                            samples[k * M + j] = incidentRadiance;
                            sampleDistances[k * M + j] = intersection2.distance();

                            radiance += incidentRadiance * M_PI / (M * N);
                        }
                        else {
                            sampleDistances[k * M + j] = FLT_MAX;
                        }
                    }
                }

                if (mean > 0) {
                    mean = den / mean;

                    IrradianceCache::Entry newEntry;
                    newEntry.point = point;
                    newEntry.normal = normal;
                    newEntry.radiance = radiance;

                    float radius = mean;
                    float projectedPixelSize = scene.camera().projectSize(2.0f / framebuffer.width(), intersection.distance());
                    float minRadius = 3 * projectedPixelSize / mIrradianceCache.threshold();
                    float maxRadius = 20 * minRadius;
                    newEntry.radius = std::min(std::max(radius, minRadius), maxRadius);

                    RadianceGradient transGrad;
                    RadianceGradient rotGrad;
                    for (int k = 0; k < N; k++) {
                        int k1 = (k > 0) ? (k - 1) : N - 1;
                        float phi = 2 * M_PI * k / N;
                        Math::Vector u = basis.localToWorld(Math::Vector::fromPolar(phi, 0, 1));
                        Math::Vector v = basis.localToWorld(Math::Vector::fromPolar(phi + M_PI / 2, 0, 1));

                        for (int j = 0; j < M; j++) {
                            float thetaMinus = std::asin(std::sqrt((float)j / M));
                            float thetaPlus = std::asin(std::sqrt((float)(j + 1) / M));

                            if (j > 0) {
                                int j1 = j - 1;

                                Math::Vector c = u * std::sin(thetaMinus) * std::cos(thetaMinus) * std::cos(thetaMinus) * 2 * M_PI / (N * std::min(sampleDistances[k * M + j], sampleDistances[k * M + j1]));
                                transGrad += RadianceGradient(samples[k * M + j] - samples[k * M + j1], c);
                            }

                            Math::Vector c = v * (std::sin(thetaPlus) - std::sin(thetaMinus)) / std::min(sampleDistances[k * M + j], sampleDistances[k1 * M + j]);
                            transGrad += RadianceGradient(samples[k * M + j] - samples[k1 * M + j], c);

                            rotGrad += RadianceGradient(samples[k * M + j], v) * std::tan(thetaMinus) * M_PI / (M * N);
                        }
                    }

                    if (radius < minRadius) {
                        transGrad = transGrad * radius / minRadius;
                    }

                    newEntry.transGrad = transGrad;
                    newEntry.rotGrad = rotGrad;

                    mIrradianceCache.add(newEntry);
                    pixelColor = Object::Color(1, 1, 1);
                }
            }
        }

        framebuffer.setPixel(x, y, pixelColor);
    }
}

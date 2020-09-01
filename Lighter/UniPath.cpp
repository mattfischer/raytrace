#include "Lighter/UniPath.hpp"

#include "Object/Scene.hpp"

namespace Lighter
{
    Object::Radiance UniPath::light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
    {
        Object::Radiance emittedRadiance = lightRadiant(intersection);
        Object::Radiance transmittedRadiance = lightTransmitted(intersection, sampler);
        Object::Radiance reflectedRadiance = lightReflected(intersection, sampler);

        return emittedRadiance + transmittedRadiance + reflectedRadiance;
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
                Object::Radiance incidentRadiance = light(intersection2, sampler, 0) * dot;
                Object::Radiance transmittedRadiance = surface.brdf().transmitted(incidentRadiance, incidentDirection, incidentNormal, albedo);
                radiance += transmittedRadiance / (outgoingDirection * normal);
            }
        }

        return radiance / threshold;
    }

    Object::Radiance UniPath::lightReflected(const Object::Intersection &intersection, Render::Sampler &sampler) const
    {
        const Math::Normal &normal = intersection.normal();
        const Math::Vector outgoingDirection = -intersection.ray().direction();
        const Object::Brdf::Composite &brdf = intersection.primitive().surface().brdf();

        Object::Radiance specularRadiance;
        if(brdf.hasSpecular()) {
            Math::Vector incidentDirection;
            float lightPdf;
            specularRadiance = sampleBrdf(intersection, brdf.specular(), sampler, incidentDirection, lightPdf);
            float pdf = brdf.specular().pdf(incidentDirection, normal, outgoingDirection);
            float diffusePdf = brdf.hasDiffuse() ? brdf.diffuse().pdf(incidentDirection, normal, outgoingDirection) : 0;
            float weight = pdf / (pdf + diffusePdf + lightPdf);
            specularRadiance = specularRadiance * weight;
        }

        Object::Radiance diffuseRadiance;
        if(brdf.hasDiffuse()) {
            Math::Vector incidentDirection;
            float lightPdf;
            diffuseRadiance = sampleBrdf(intersection, brdf.diffuse(), sampler, incidentDirection, lightPdf);
            float pdf = brdf.diffuse().pdf(incidentDirection, normal, outgoingDirection);
            float specularPdf = brdf.hasSpecular() ? brdf.specular().pdf(incidentDirection, normal, outgoingDirection) : 0;
            float weight = pdf / (pdf + specularPdf + lightPdf);
            diffuseRadiance = diffuseRadiance * weight;
        }

        Object::Radiance lightRadiance;
        for (const Object::Primitive &light : intersection.scene().areaLights()) {
            Math::Vector incidentDirection;
            float pdf;
            lightRadiance += sampleLight(intersection, light, sampler, incidentDirection, pdf);
            float specularPdf = brdf.hasSpecular() ? brdf.specular().pdf(incidentDirection, normal, outgoingDirection) : 0;
            float diffusePdf = brdf.hasDiffuse() ? brdf.diffuse().pdf(incidentDirection, normal, outgoingDirection) : 0;
            float weight = pdf / (pdf + specularPdf + diffusePdf);
            lightRadiance = lightRadiance * weight;
        }

        for (const std::unique_ptr<Object::PointLight> &pointLight : intersection.scene().pointLights()) {
            Math::Vector incidentDirection;
            lightRadiance += evaluatePointLight(intersection, *pointLight, incidentDirection);
        }

        Object::Radiance radiance = specularRadiance + diffuseRadiance + lightRadiance;

        return radiance;
    }

    Object::Radiance UniPath::sampleLight(const Object::Intersection &intersection, const Object::Primitive &light, Render::Sampler &sampler, Math::Vector &incidentDirection, float &pdfAngular) const
    {
        Object::Radiance radiance;

        const Math::Point &point = intersection.point();
        const Math::Normal &normal = intersection.normal();
        const Object::Scene &scene = intersection.scene();
        const Object::Brdf::Composite &brdf = intersection.primitive().surface().brdf();
        const Math::Vector &outgoingDirection = -intersection.ray().direction();
        const Object::Color &albedo = intersection.albedo();

        const Object::Radiance &objectRadiance = light.surface().radiance();
        const Object::Shape::Base::Sampler *shapeSampler = light.shape().sampler();
        if (!shapeSampler) {
            pdfAngular = 0;
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
        float pdf = 1.0f / surfaceArea;
        float sampleDot = std::max(std::abs(incidentDirection * sampleNormal), 0.01f);

        pdfAngular = (pdf * distance * distance) / sampleDot;

        float dot = incidentDirection * normal;
        if (dot > 0) {
            Math::Point offsetPoint = point + Math::Vector(normal) * 0.0001f;
            Math::Ray ray(offsetPoint, incidentDirection);
            Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
            Object::Intersection intersection2 = scene.intersect(beam);

            if (intersection2.valid() && &(intersection2.primitive()) == &light) {
                Object::Radiance incidentRadiance = objectRadiance * sampleDot * dot / (distance * distance);
                radiance += brdf.reflected(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / pdf;
            }
        }

        return radiance;
    }

    Object::Radiance UniPath::evaluatePointLight(const Object::Intersection &intersection, const Object::PointLight &pointLight, Math::Vector &incidentDirection) const
    {
        Object::Radiance radiance;

        const Math::Point &point = intersection.point();
        const Math::Normal &normal = intersection.normal();
        const Object::Scene &scene = intersection.scene();
        const Object::Brdf::Composite &brdf = intersection.primitive().surface().brdf();
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
            if (dot > 0) {
                Object::Radiance incidentRadiance = pointLight.radiance() * dot / (distance * distance);
                radiance += brdf.reflected(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo);
            }
        }

        return radiance;
    }

    Object::Radiance UniPath::sampleBrdf(const Object::Intersection &intersection, const Object::Brdf::Base &brdf, Render::Sampler &sampler, Math::Vector &incidentDirection, float &lightPdf) const
    {
        const Object::Scene &scene = intersection.scene();
        const Object::Color &albedo = intersection.albedo();
        const Object::Surface &surface = intersection.primitive().surface();
        const Math::Ray &ray = intersection.ray();
        const Math::Normal &normal = intersection.facingNormal();
        Math::Vector outgoingDirection = -ray.direction();

        Object::Radiance radiance;

        Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01f;

        Math::Point2D samplePoint = sampler.getValue2D();
        incidentDirection = brdf.sample(samplePoint, normal, outgoingDirection);

        lightPdf = 0;
        float pdf = brdf.pdf(incidentDirection, normal, outgoingDirection);

        Object::Radiance testRadiance = surface.brdf().reflected(Object::Radiance(1,1,1) / std::sqrt(3.0f), incidentDirection, normal, outgoingDirection, albedo) / pdf;
        float threshold = std::max(std::min(testRadiance.magnitude(), 0.9f), 0.1f);
        float roulette = sampler.getValue();
        if(roulette > threshold) {
            return radiance;
        }

        float dot = incidentDirection * normal;
        if(dot > 0) {
            Math::Ray reflectRay(offsetPoint, incidentDirection);
            Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
            Object::Intersection intersection2 = scene.intersect(beam);

            if (intersection2.valid()) {
                Object::Radiance incidentRadiance = light(intersection2, sampler, 0) * dot;
                radiance = surface.brdf().reflected(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / pdf;
                if(intersection2.primitive().surface().radiance().magnitude() > 0) {
                    const Object::Shape::Base::Sampler *shapeSampler = intersection2.primitive().shape().sampler();
                    if (shapeSampler) {
                        float areaPdf = 1.0f / shapeSampler->surfaceArea();
                        float lightDot = std::max(std::abs(intersection2.normal() * incidentDirection), 0.01f);
                        lightPdf = (areaPdf * intersection2.distance() * intersection2.distance()) / lightDot;
                    }
                }
            }
        }

        return radiance / threshold;
    }
}

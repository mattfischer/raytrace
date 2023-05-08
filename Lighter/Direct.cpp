#define _USE_MATH_DEFINES
#include "Lighter/Direct.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <cfloat>

namespace Lighter
{
    Object::Radiance Direct::light(const Object::Intersection &intersection, Render::Sampler &sampler) const
    {
        const Object::Scene &scene = intersection.scene();
        const Object::Surface &surface = intersection.primitive().surface();
        const Math::Point &point = intersection.point();
        const Math::Normal &facingNormal = surface.facingNormal(intersection);
        const Object::Color &albedo = surface.albedo(intersection);

        Object::Radiance radiance = surface.radiance();
        Math::Point offsetPoint = intersection.point() + Math::Vector(facingNormal) * 0.01f;

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

            float dot = incidentDirection * facingNormal;
            if(dot > 0) {
                Math::Ray ray(offsetPoint, incidentDirection);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection intersection2 = scene.intersect(beam);

                if (intersection2.valid() && &(intersection2.primitive()) == &light) {
                    Object::Radiance irradiance = objectRadiance * sampleDot * dot / (distance * distance);
                    radiance += irradiance * surface.reflected(intersection, incidentDirection) / pdf;
                }
            }
        }

        for (const std::unique_ptr<Object::PointLight> &pointLight : intersection.scene().pointLights()) {
            Math::Vector incidentDirection = pointLight->position() - offsetPoint;
            float distance = incidentDirection.magnitude();
            incidentDirection = incidentDirection / distance;

            float dot = incidentDirection * facingNormal;
            if(dot > 0) {
                Math::Ray ray(offsetPoint, incidentDirection);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection intersection2 = scene.intersect(beam);

                if (!intersection2.valid() || intersection2.distance() >= distance) {
                    Object::Radiance irradiance = pointLight->radiance() * dot / (distance * distance);
                    radiance += irradiance * surface.reflected(intersection, incidentDirection);
                }
            }
        }

        return radiance;
    }
}


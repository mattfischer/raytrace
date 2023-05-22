#define _USE_MATH_DEFINES
#include "Lighter/Direct.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <cfloat>

namespace Lighter
{
    Object::Radiance Direct::light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const
    {
        const Object::Scene &scene = isect.scene();
        const Object::Surface &surface = isect.primitive().surface();
        const Math::Point &point = isect.point();
        const Math::Normal &nrmFacing = surface.facingNormal(isect);

        Object::Radiance rad = surface.radiance();
        Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

        for (const Object::Primitive &light : scene.areaLights()) {
            const Object::Radiance &rad2 = light.surface().radiance();

            Math::Point pntSample;
            Math::Normal nrmSample;
            float pdf;

            if(!light.shape().sample(sampler, pntSample, nrmSample, pdf)) {
                continue;
            }

            Math::Vector dirIn = pntSample - pntOffset;
            float d = dirIn.magnitude();
            dirIn = dirIn / d;
            float dotSample = std::abs(dirIn * nrmSample);

            float dot = dirIn * nrmFacing;
            if(dot > 0) {
                Math::Ray ray(pntOffset, dirIn);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection isect2 = scene.intersect(beam);

                if (isect2.valid() && &(isect2.primitive()) == &light) {
                    Object::Radiance irad = rad2 * dotSample * dot / (d * d);
                    rad += irad * surface.reflected(isect, dirIn) / pdf;
                }
            }
        }

        for (const std::unique_ptr<Object::PointLight> &pointLight : scene.pointLights()) {
            Math::Vector dirIn = pointLight->position() - pntOffset;
            float d = dirIn.magnitude();
            dirIn = dirIn / d;

            float dot = dirIn * nrmFacing;
            if(dot > 0) {
                Math::Ray ray(pntOffset, dirIn);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection isect2 = scene.intersect(beam);

                if (!isect2.valid() || isect2.distance() >= d) {
                    Object::Radiance irad = pointLight->radiance() * dot / (d * d);
                    rad += irad * surface.reflected(isect, dirIn);
                }
            }
        }

        return rad;
    }
}


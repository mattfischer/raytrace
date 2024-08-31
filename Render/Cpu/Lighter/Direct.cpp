#define _USE_MATH_DEFINES
#include "Render/Cpu/Lighter/Direct.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <cfloat>

namespace Render::Cpu::Lighter {
    Math::Radiance Direct::light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const
    {
        const Object::Scene &scene = isect.scene();
        const Object::Surface &surface = isect.primitive().surface();
        const Math::Point &point = isect.point();
        const Math::Normal &nrmFacing = isect.facingNormal();

        Math::Radiance rad = surface.radiance();
        Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

        for (const Object::Primitive &light : scene.areaLights()) {
            const Math::Radiance &rad2 = light.surface().radiance();

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
                Object::Intersection isect2 = scene.intersect(beam, d, false);

                if (!isect2.valid() || &(isect2.primitive()) == &light) {
                    Math::Radiance irad = rad2 * dotSample * dot / (d * d);
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
                Object::Intersection isect2 = scene.intersect(beam, d, false);

                if (!isect2.valid()) {
                    Math::Radiance irad = pointLight->radiance() * dot / (d * d);
                    rad += irad * surface.reflected(isect, dirIn);
                }
            }
        }

        return rad;
    }
}
#define _USE_MATH_DEFINES
#include "Render/Cpu/Impl/Lighter/Direct.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <cfloat>

namespace Render::Cpu::Impl::Lighter {
    Math::Radiance Direct::light(const Object::Intersection &isect, Math::Sampler &sampler) const
    {
        const Object::Scene &scene = isect.scene();
        const Object::Surface &surface = isect.primitive().surface();
        const Math::Point &point = isect.point();
        const Math::Normal &nrmFacing = isect.facingNormal();

        Math::Radiance rad;
        if(isect.primitive().light()) {
            rad += isect.primitive().light()->radiance(isect);
        }

        Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

        for(const Object::Light &light : scene.lights()) {
            Object::Light::Sample sample = light.sample(sampler, pntOffset);

            float dot = sample.direction * nrmFacing;
            if(dot > 0 && light.testVisible(scene, sample)) {
                Math::Radiance irad = sample.radiance * dot;
                rad += irad * surface.reflected(isect, sample.direction) / sample.pdf;
            }
        }

        return rad;
    }
}
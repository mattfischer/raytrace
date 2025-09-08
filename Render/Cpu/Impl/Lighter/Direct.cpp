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
            auto [radLight, dirIn, pdf] = light.sample(sampler, pntOffset);

            float dot = dirIn * nrmFacing;
            if(dot > 0 && light.testVisible(scene, pntOffset, dirIn)) {
                Math::Radiance irad = radLight * dot;
                rad += irad * surface.reflected(isect, dirIn) / pdf;
            }
        }

        return rad;
    }
}
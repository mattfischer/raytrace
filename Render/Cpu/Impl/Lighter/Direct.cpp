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

        Math::Radiance rad = surface.radiance();
        Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

        for(const std::unique_ptr<Object::Light> &light : scene.lights()) {
            Math::Point pntSample;
            float dotSample;
            Math::Pdf pdf;
            Math::Radiance rad2 = light->sample(sampler, pntOffset, pntSample, dotSample, pdf);

            Math::Vector dirIn = pntSample - pntOffset;
            float d = dirIn.magnitude();
            dirIn = dirIn / d;

            float dot = dirIn * nrmFacing;
            if(dot > 0) {
                Math::Ray ray(pntOffset, dirIn);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection isect2 = scene.intersect(beam, d, false);

                if (!isect2.valid() || light->didIntersect(isect2)) {
                    Math::Radiance irad = rad2 * dot / (d * d);
                    rad += irad * surface.reflected(isect, dirIn) / pdf;
                }
            }
        }

        return rad;
    }
}
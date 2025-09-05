#define _USE_MATH_DEFINES
#include "Render/Cpu/Impl/Lighter/UniPath.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <algorithm>

namespace Render::Cpu::Impl::Lighter {
    Math::Radiance UniPath::light(const Object::Intersection &isectBase, Math::Sampler &sampler) const
    {
        Object::Intersection isect = isectBase;
        Math::Radiance rad = isect.primitive().surface().radiance();
        Math::Color throughput(1, 1, 1);

        for(int generation = 0; generation < 10; generation++) {
            const Object::Surface &surface = isect.primitive().surface();
            const Object::Scene &scene = isect.scene();
            const Math::Normal &nrmFacing = isect.facingNormal();
            const Math::Vector dirOut = -isect.ray().direction();

            Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

            for(const std::unique_ptr<Object::Light> &light : scene.lights()) {
                Math::Point pntSample;
                Math::Pdf pdf;
                Math::Radiance rad2 = light->sample(sampler, pntOffset, pntSample, pdf);

                Math::Vector dirIn = pntSample - pntOffset;
                float d = dirIn.magnitude();
                dirIn = dirIn / d;

                float dot = dirIn * nrmFacing;
                if(dot > 0) {
                    Math::Ray ray(pntOffset, dirIn);
                    Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                    Object::Intersection isect2 = scene.intersect(beam, d, false);

                    if (!isect2.valid() || light->didIntersect(isect2)) {
                        Math::Radiance irad = rad2 * dot;
                        float pdfBrdf = pdf.isDelta() ? 0.0f : surface.pdf(isect, dirIn);
                        float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                        
                        rad += irad * surface.reflected(isect, dirIn) * throughput * misWeight / pdf;
                    }
                }
            }

            Math::Vector dirIn;
            Math::Pdf pdf;
            Math::Color reflected = surface.sample(isect, sampler, dirIn, pdf);
            float reverse = (dirIn * nrmFacing > 0) ? 1.0f : -1.0f;
            float dot = dirIn * nrmFacing * reverse;

            pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f * reverse;

            if(dot <= 0) {
                break;
            }

            float threshold = 1.0f;
            float roulette = sampler.getValue();
            if(generation > 0) {
                threshold = std::min(1.0f, throughput.maximum());
            }

            if(roulette >= threshold) {
                break;
            }

            throughput = throughput * reflected * dot / (pdf * threshold);

            Math::Ray reflectRay(pntOffset, dirIn);
            Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
            Object::Intersection isect2 = scene.intersect(beam, FLT_MAX, true);

            if (isect2.valid()) {
                Math::Radiance rad2 = isect2.primitive().surface().radiance();
                if(rad2.magnitude() > 0 && !pdf.isDelta()) {
                    float dot2 = -isect2.facingNormal() * dirIn;
                    float pdfLight = isect2.primitive().shape().samplePdf(isect2.point()) * isect2.distance() * isect2.distance() / dot2;
                    float misWeight = pdf * pdf / (pdf * pdf + pdfLight * pdfLight);
        
                    rad += rad2 * throughput * misWeight;
                }

                isect = isect2;
            } else {
                Math::Radiance rad2 = scene.skyRadiance();
                rad += rad2 * throughput;
                break;
            }
        }

        return rad;
    }
}
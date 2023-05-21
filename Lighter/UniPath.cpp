#define _USE_MATH_DEFINES
#include "Lighter/UniPath.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <algorithm>

namespace Lighter
{
    Object::Radiance UniPath::light(const Object::Intersection &isect, Render::Sampler &sampler) const
    {
        return lightInternal(isect, sampler, 0);
    }

    Object::Radiance UniPath::lightInternal(const Object::Intersection &isect, Render::Sampler &sampler, int generation) const
    {
        const Object::Surface &surface = isect.primitive().surface();
        const Object::Scene &scene = isect.scene();
        const Math::Normal &nrmFacing = surface.facingNormal(isect);
        const Math::Vector dirOut = -isect.ray().direction();
        
        Object::Radiance rad = isect.primitive().surface().radiance();

        Math::Vector dirIn;
        float pdf;
        bool pdfDelta;
        Object::Color reflected = surface.sample(isect, sampler, dirIn, pdf, pdfDelta);
        float reverse = (dirIn * nrmFacing > 0) ? 1.0f : -1.0f;
        float dot = dirIn * nrmFacing * reverse;

        Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f * reverse;

        if(dot > 0) {
            Object::Color throughput = reflected * dot / pdf;

            float threshold = 0.0f;
            float roulette = sampler.getValue();
            if(generation == 0) {
                threshold = 1.0f;
            } else if(generation < 10) {
                threshold = std::min(1.0f, throughput.maximum());
            }

            if(roulette < threshold) {
                Math::Ray reflectRay(pntOffset, dirIn);
                Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
                Object::Intersection isect2 = scene.intersect(beam);

                if (isect2.valid()) {
                    Object::Radiance rad2 = lightInternal(isect2, sampler, generation + 1);
                    float misWeight = 1.0f;
                    if(isect2.primitive().surface().radiance().magnitude() > 0 && !pdfDelta) {
                        float dot2 = -isect2.primitive().surface().facingNormal(isect2) * dirIn;
                        float pdfArea = pdf * dot2 / (isect2.distance() * isect2.distance());
                        float pdfLight = 0.0f;
                        const Object::Shape::Base::Sampler *shapeSampler = isect2.primitive().shape().sampler();
                        if (shapeSampler) {
                            pdfLight = 1.0f / shapeSampler->surfaceArea();
                        }
                        misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
                    }
                    Object::Radiance radNew = rad2 * throughput / threshold;
                    rad += radNew * misWeight;
                }
            }
        }

        pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

        for (const Object::Primitive &light : scene.areaLights()) {
            const Object::Radiance &rad2 = light.surface().radiance();
            const Object::Shape::Base::Sampler *shapeSampler = light.shape().sampler();

            if (!shapeSampler) {
                continue;
            }

            Math::Point pntSample;
            Math::Normal nrmSample;

            shapeSampler->sample(sampler.getValue2D(), pntSample, nrmSample);

            Math::Vector dirIn = pntSample - pntOffset;
            float d = dirIn.magnitude();
            dirIn = dirIn / d;
            float pdf = 1.0f / shapeSampler->surfaceArea();
            float dotSample = std::abs(dirIn * nrmSample);

            float dot = dirIn * nrmFacing;
            if(dot > 0) {
                Math::Ray ray(pntOffset, dirIn);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection isect2 = scene.intersect(beam);

                if (isect2.valid() && &(isect2.primitive()) == &light) {
                    Object::Radiance irad = rad2 * dotSample * dot / (d * d);
                    Object::Radiance radNew = irad * surface.reflected(isect, dirIn) / pdf;
                    float pdfBrdf = surface.pdf(isect, dirIn) * dotSample / (d * d);
                    float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                    rad += radNew * misWeight;
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

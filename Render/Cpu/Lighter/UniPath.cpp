#define _USE_MATH_DEFINES
#include "Render/Cpu/Lighter/UniPath.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <algorithm>

namespace Render {
    namespace Cpu {
        namespace Lighter
        {
            Object::Radiance UniPath::light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const
            {
                return lightInternal(isect, sampler, 0);
            }

            Object::Radiance UniPath::lightInternal(const Object::Intersection &isect, Math::Sampler::Base &sampler, int generation) const
            {
                const Object::Surface &surface = isect.primitive().surface();
                const Object::Scene &scene = isect.scene();
                const Math::Normal &nrmFacing = isect.facingNormal();
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

                        Object::Radiance rad2;
                        float misWeight = 1.0f;
                        if (isect2.valid()) {
                            rad2 = lightInternal(isect2, sampler, generation + 1);
                            if(isect2.primitive().surface().radiance().magnitude() > 0 && !pdfDelta) {
                                float dot2 = -isect2.facingNormal() * dirIn;
                                float pdfArea = pdf * dot2 / (isect2.distance() * isect2.distance());
                                float pdfLight = isect2.primitive().shape().samplePdf(isect2.point());
                                misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
                            }
                        } else {
                            rad2 = scene.skyRadiance();
                        }

                        rad += rad2 * throughput * misWeight / threshold;
                    }
                }

                pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

                for (const Object::Primitive &light : scene.areaLights()) {
                    const Object::Radiance &rad2 = light.surface().radiance();
                    
                    Math::Point pnt2;
                    Math::Normal nrm2;
                    float pdf;
                    if(!light.shape().sample(sampler, pnt2, nrm2, pdf)) {
                        continue;
                    }

                    Math::Vector dirIn = pnt2 - pntOffset;
                    float d = dirIn.magnitude();
                    dirIn = dirIn / d;
                    float dot2 = std::abs(dirIn * nrm2);

                    float dot = dirIn * nrmFacing;
                    if(dot > 0) {
                        Math::Ray ray(pntOffset, dirIn);
                        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                        Object::Intersection isect2 = scene.intersect(beam);

                        if (isect2.valid() && &(isect2.primitive()) == &light) {
                            Object::Radiance irad = rad2 * dot2 * dot / (d * d);
                            float pdfBrdf = surface.pdf(isect, dirIn) * dot2 / (d * d);
                            float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                            rad += irad * surface.reflected(isect, dirIn) * misWeight / pdf;
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
    }
}
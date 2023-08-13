#define _USE_MATH_DEFINES
#include "Render/Cpu/Lighter/UniPath.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <algorithm>

namespace Render {
    namespace Cpu {
        namespace Lighter
        {
            static const int kNumRisSamples = 4;

            Math::Radiance UniPath::light(const Object::Intersection &isectBase, Math::Sampler::Base &sampler) const
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

                    for (const Object::Primitive &light : scene.areaLights()) {
                        const Math::Radiance &rad2 = light.surface().radiance();
                        
                        Math::Vector dirIn;
                        Math::Radiance radUnshadowed;
                        float risWeightTotal = 0;

                        int numRisSamples = (generation == 0) ? kNumRisSamples : 1;

                        for(int i=0; i<numRisSamples; i++) {
                            Math::Point pnt2;
                            Math::Normal nrm2;
                            float pdf;
                            if(!light.shape().sample(sampler, pnt2, nrm2, pdf)) {
                                continue;
                            }

                            Math::Vector dirInProp = pnt2 - pntOffset;
                            float d = dirInProp.magnitude();
                            dirInProp = dirInProp / d;
                            float dot2 = std::abs(dirInProp * nrm2);

                            float dot = dirInProp * nrmFacing;
                            if(dot <= 0) {
                                continue;
                            }

                            Math::Radiance irad = rad2 * dot2 * dot / (d * d);
                            float pdfBrdf = surface.pdf(isect, dirInProp) * dot2 / (d * d);
                            float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                            Math::Radiance radProp = irad * surface.reflected(isect, dirInProp) * throughput * misWeight;
                            
                            float q = radProp.magnitude();
                            float w = q / pdf;
                            risWeightTotal += w;
                            if(i == 0 || sampler.getValue() < w / risWeightTotal) {
                                dirIn = dirInProp;
                                radUnshadowed = radProp / q;
                            }
                        }

                        if(risWeightTotal > 0) {
                            Math::Ray ray(pntOffset, dirIn);
                            Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                            Object::Intersection isect2 = scene.intersect(beam);

                            if (isect2.valid() && &(isect2.primitive()) == &light) {
                                rad += radUnshadowed * risWeightTotal / numRisSamples;
                            }
                        }
                    }

                    for (const std::unique_ptr<Object::PointLight> &pointLight : scene.pointLights()) {
                        Math::Vector dirIn = pointLight->position() - pntOffset;
                        float d = dirIn.magnitude();
                        dirIn = dirIn / d;

                        float dot = dirIn * nrmFacing;
                        if(dot <= 0) {
                            continue;
                        }

                        Math::Ray ray(pntOffset, dirIn);
                        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                        Object::Intersection isect2 = scene.intersect(beam);

                        if (!isect2.valid() || isect2.distance() >= d) {
                            Math::Radiance irad = pointLight->radiance() * dot / (d * d);
                            rad += irad * surface.reflected(isect, dirIn) * throughput;
                        }
                    }

                    Math::Vector dirIn;
                    float pdf;
                    bool pdfDelta;
                    Math::Color reflected = surface.sample(isect, sampler, dirIn, pdf, pdfDelta);
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
                    Object::Intersection isect2 = scene.intersect(beam);

                    if (isect2.valid()) {
                        Math::Radiance rad2 = isect2.primitive().surface().radiance();
                        if(rad2.magnitude() > 0 && !pdfDelta) {
                            float dot2 = -isect2.facingNormal() * dirIn;
                            float pdfArea = pdf * dot2 / (isect2.distance() * isect2.distance());
                            float pdfLight = isect2.primitive().shape().samplePdf(isect2.point());
                            float misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
                
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
    }
}
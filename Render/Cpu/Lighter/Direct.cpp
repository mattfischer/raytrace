#define _USE_MATH_DEFINES
#include "Render/Cpu/Lighter/Direct.hpp"

#include "Object/Scene.hpp"

#include <cmath>
#include <cfloat>

namespace Render {
    namespace Cpu {
        namespace Lighter {
            static const int kNumRisSamples = 4;

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

                    Math::Vector dirIn;
                    Math::Radiance radUnshadowed;
                    float risWeightTotal = 0;

                    for(int i=0; i<kNumRisSamples; i++) {    
                        Math::Point pntSample;
                        Math::Normal nrmSample;
                        float pdf;

                        if(!light.shape().sample(sampler, pntSample, nrmSample, pdf)) {
                            continue;
                        }

                        Math::Vector dirInProp = pntSample - pntOffset;
                        float d = dirInProp.magnitude();
                        dirInProp = dirInProp / d;

                        float dot = dirInProp * nrmFacing;
                        if(dot <= 0) {
                            continue;
                        }

                        float dotSample = std::abs(dirInProp * nrmSample);
                        Math::Radiance irad = rad2 * dotSample * dot / (d * d);
                        Math::Radiance radProp = irad * surface.reflected(isect, dirInProp);

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
                            rad += radUnshadowed * risWeightTotal / kNumRisSamples;
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
                            Math::Radiance irad = pointLight->radiance() * dot / (d * d);
                            rad += irad * surface.reflected(isect, dirIn);
                        }
                    }
                }

                return rad;
            }
        }
    }
}

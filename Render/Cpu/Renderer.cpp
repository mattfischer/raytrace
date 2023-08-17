#include "Render/Cpu/Renderer.hpp"
#include "Render/Cpu/RasterJob.hpp"

#include "Math/Sampler/Halton.hpp"

#include "Render/Cpu/Lighter/UniPath.hpp"

#include <atomic>
#include <mutex>

namespace Render {
    namespace Cpu {
        struct ThreadLocal : public Executor::Job::ThreadLocal {
            Math::Sampler::Halton sampler;

            ThreadLocal(int width, int height) : sampler(width, height) {}
        };
            
        Renderer::Renderer(const Object::Scene &scene, const Settings &settings, std::unique_ptr<Lighter::Base> lighter)
        : mScene(scene)
        , mSettings(settings)
        , mDirectReservoirs(settings.width, settings.height)
        , mIndirectReservoirs(settings.width, settings.height)
        , mPrimaryHits(settings.width, settings.height)
        , mTotalRadiance(settings.width, settings.height)
        {
            mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
            mSampleStatusFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);

            mIndirectLighter = std::make_unique<Render::Cpu::Lighter::UniPath>();
        }

        void Renderer::start(Listener *listener)
        {
            mListener = listener;
            mStartTime = std::chrono::steady_clock::now();

            mCurrentSample = 0;
            startRenderJob();
        }

        void Renderer::stop()
        {
            mExecutor.stop();
        }

        bool Renderer::running()
        {
            return mExecutor.running();
        }

        Render::Framebuffer &Renderer::renderFramebuffer()
        {
            return *mRenderFramebuffer;
        }

        Render::Framebuffer &Renderer::sampleStatusFramebuffer()
        {
            return *mSampleStatusFramebuffer;
        }

        void Renderer::renderJobDone()
        {
            startSampleDirectJob();
        }

        void Renderer::sampleDirectJobDone()
        {
            startSampleIndirectJob();
        }

        void Renderer::sampleIndirectJobDone()
        {
            mCurrentSample++;
            if(mCurrentSample < mSettings.minSamples) {
                startRenderJob();
            } else {
                auto endTime = std::chrono::steady_clock::now();
                std::chrono::duration<double> duration = endTime - mStartTime;
                mListener->onRendererDone(duration.count());
            }
        }

        void Renderer::startRenderJob()
        {
            std::unique_ptr<Executor::Job> job = 
                std::make_unique<RasterJob>(
                    mSettings.width,
                    mSettings.height,
                    1,
                    [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height()); },
                    [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                        {
                            renderPixel(x, y, mCurrentSample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                        }
                );

            mExecutor.runJob(std::move(job), [&]() { renderJobDone(); });
        }

        void Renderer::startSampleDirectJob()
        {
            std::unique_ptr<Executor::Job> job = 
                std::make_unique<RasterJob>(
                    mSettings.width,
                    mSettings.height,
                    1,
                    [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height()); },
                    [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                        {
                            sampleDirectPixel(x, y, mCurrentSample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                        }
                );

            mExecutor.runJob(std::move(job), [&]() { sampleDirectJobDone(); });
        }

        void Renderer::startSampleIndirectJob()
        {
            std::unique_ptr<Executor::Job> job = 
                std::make_unique<RasterJob>(
                    mSettings.width,
                    mSettings.height,
                    1,
                    [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height()); },
                    [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                        {
                            sampleIndirectPixel(x, y, mCurrentSample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                        }
                );

            mExecutor.runJob(std::move(job), [&]() { sampleIndirectJobDone(); });
        }

        void Renderer::renderPixel(int x, int y, int sample, Math::Sampler::Base &sampler)
        {
            Math::Bivector dv;
            sampler.startSample(x, y, sample);
            Math::Point2D imagePoint = Math::Point2D((float)x, (float)y) + sampler.getValue2D();
            Math::Point2D aperturePoint = sampler.getValue2D();
            Math::Beam beam = mScene.camera().createPixelBeam(imagePoint, mSettings.width, mSettings.height, aperturePoint);
            Object::Intersection isect = mScene.intersect(beam);

            PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
            primaryHit.beam = beam;
            primaryHit.isect = Object::Intersection(mScene, isect.primitive(), primaryHit.beam, isect.shapeIntersection());

            mDirectReservoirs.at(x, y).weight = 0;
            mIndirectReservoirs.at(x, y).weight = 0;
    
            Math::Radiance rad;
            if (isect.valid()) {
                const Math::Normal &nrmFacing = isect.facingNormal(); 
                const Object::Surface &surface = isect.primitive().surface();
   
                Math::Radiance radEmitted = isect.primitive().surface().radiance();
                
                float misWeightLight = 0;
                Math::Radiance radDirectLight;

                int lightIndex = (int)std::floor(sampler.getValue() * mScene.areaLights().size());
                const Object::Primitive &light = mScene.areaLights()[lightIndex];
                const Math::Radiance &rad2 = light.surface().radiance();

                Math::Point pnt2;
                Math::Normal nrm2;
                float pdf2;

                if(light.shape().sample(sampler, pnt2, nrm2, pdf2)) {
                    Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

                    Math::Vector dirIn = pnt2 - pntOffset;
                    float d = dirIn.magnitude();
                    dirIn = dirIn / d;
                    float dot2 = std::abs(dirIn * nrm2);

                    float dot = dirIn * nrmFacing;
                    if(dot > 0) {
                        Math::Radiance irad = rad2 * dot2 * dot / (d * d);
                        radDirectLight = irad * surface.reflected(isect, dirIn);
        
                        DirectReservoir &reservoir = mDirectReservoirs.at(x, y);
                        reservoir.point = pnt2;
                        reservoir.radiance = rad2;
                        reservoir.normal = nrm2;
                        reservoir.primitive = &light;
                        float q = radDirectLight.magnitude();
                        reservoir.weight = q / pdf2;
                    }
                }

                Math::Vector dirIn;
                float pdf;
                bool pdfDelta;
                Math::Color reflected = surface.sample(isect, sampler, dirIn, pdf, pdfDelta);
                float reverse = (dirIn * nrmFacing > 0) ? 1.0f : -1.0f;
                float dot = dirIn * nrmFacing * reverse;

                Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f * reverse;

                if(dot > 0) {
                    Math::Ray reflectRay(pntOffset, dirIn);
                    Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
                    Object::Intersection isect2 = mScene.intersect(beam);

                    if (isect2.valid()) {
                        Math::Radiance rad2 = mIndirectLighter->light(isect2, sampler);
                  
                        IndirectReservoir &reservoir = mIndirectReservoirs.at(x, y);
                        reservoir.dirIn = dirIn;
                        reservoir.indirectRadiance = (rad2 - isect2.primitive().surface().radiance()); 
                        float q = reservoir.indirectRadiance.magnitude();
                        reservoir.weight = q / pdf;
                    }
                }
                rad = radEmitted;
            } else {
                rad = mScene.skyRadiance();
            }
    
            Math::Radiance radTotal = mTotalRadiance.get(x, y) + rad;
            mTotalRadiance.set(x, y, radTotal);
            Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
            mRenderFramebuffer->setPixel(x, y, color);
        }

        void Renderer::sampleDirectPixel(int x, int y, int sample, Math::Sampler::Base &sampler)
        {
            Math::Radiance radDirect;
            PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
            const Math::Normal &nrmFacing = primaryHit.isect.facingNormal(); 
            const Object::Surface &surface = primaryHit.isect.primitive().surface();

            struct Entry {
                Math::Point point;
                Math::Radiance radiance;
                Math::Normal normal;
                const Object::Primitive *primitive;
            };
            const int N = 1;
            Entry entries[N];
            float W = 0;
            int M = 0;
            while(M < 30) {
                Math::Point2D s = sampler.getValue2D();
                s = s * 30 + Math::Point2D(-15, -15);
                int sx = (int)std::floor(s.u() + x);
                int sy = (int)std::floor(s.v() + y);
                if(sx < 0 || sy < 0 || sx >= mSettings.width || sy >= mSettings.height) {
                    continue;
                }
                DirectReservoir &reservoir = mDirectReservoirs.at(sx, sy);
                if(reservoir.weight == 0) {
                    //continue;
                    break;
                }

                W += reservoir.weight;
                if(M < N) {
                    entries[M].point = reservoir.point;
                    entries[M].radiance = reservoir.radiance;
                    entries[M].normal = reservoir.normal;
                    entries[M].primitive = reservoir.primitive;
                } else if(sampler.getValue() < reservoir.weight / W) {
                    int m = (int)std::floor(sampler.getValue() * N);
                    entries[m].point = reservoir.point;
                    entries[m].radiance = reservoir.radiance;
                    entries[m].normal = reservoir.normal;
                    entries[m].primitive = reservoir.primitive;
                }

                M++;
            }
            
            if(M == 0) {
                return;
            }

            for(int i=0; i<N; i++) {
                Math::Point pntOffset = primaryHit.isect.point() + Math::Vector(nrmFacing) * 0.01f;

                Math::Vector dirIn = entries[i].point - pntOffset;
                float d = dirIn.magnitude();
                dirIn = dirIn / d;
                float dot2 = std::abs(dirIn * entries[i].normal);

                float dot = dirIn * nrmFacing;
                
                if(dot > 0 && W > 0) {
                    Math::Ray ray(pntOffset, dirIn);
                    Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                    Object::Intersection isect2 = mScene.intersect(beam);

                    if (isect2.valid() && &(isect2.primitive()) == entries[i].primitive) {
                        Math::Radiance irad = entries[i].radiance * dot2 * dot / (d * d);
                        Math::Radiance rad = irad * surface.reflected(primaryHit.isect, dirIn);
                        float q = rad.magnitude();
                        radDirect += rad * W / (q * M * N);
                    }
                }
            }

            Math::Radiance radTotal = mTotalRadiance.get(x, y) + radDirect;
            mTotalRadiance.set(x, y, radTotal);
            Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
            mRenderFramebuffer->setPixel(x, y, color);
        }

        void Renderer::sampleIndirectPixel(int x, int y, int sample, Math::Sampler::Base &sampler)
        {
            Math::Radiance radIndirect;
            PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
            const Math::Normal &nrmFacing = primaryHit.isect.facingNormal(); 
            const Object::Surface &surface = primaryHit.isect.primitive().surface();

            struct Entry {
                Math::Vector dirIn;
                Math::Radiance indirectRadiance;
            };
            const int N = 10;
            Entry entries[N];
            float W = 0;
            int M = 0;
            while(M < 30) {
                Math::Point2D s = sampler.getValue2D();
                s = s * 30 + Math::Point2D(-15, -15);
                int sx = (int)std::floor(s.u() + x);
                int sy = (int)std::floor(s.v() + y);
                if(sx < 0 || sy < 0 || sx >= mSettings.width || sy >= mSettings.height) {
                    continue;
                }
                IndirectReservoir &reservoir = mIndirectReservoirs.at(sx, sy);
                if(reservoir.weight == 0) {
                    continue;
                }

                W += reservoir.weight;
                if(M < N) {
                    entries[M].dirIn = reservoir.dirIn;
                    entries[M].indirectRadiance = reservoir.indirectRadiance;
                } else if(sampler.getValue() < reservoir.weight / W) {
                    int m = (int)std::floor(sampler.getValue() * N);
                    entries[m].dirIn = reservoir.dirIn;
                    entries[m].indirectRadiance = reservoir.indirectRadiance;
                }

                M++;
            }
            
            for(int i=0; i<N; i++) {
                float q = entries[i].indirectRadiance.magnitude();
                float dot = entries[i].dirIn * nrmFacing;
                if(dot > 0 && W > 0) {
                    Math::Color reflected = surface.reflected(primaryHit.isect, entries[i].dirIn);
                    radIndirect += entries[i].indirectRadiance * dot * reflected * W / (q * M * N);
                }
            }

            Math::Radiance radTotal = mTotalRadiance.get(x, y) + radIndirect;
            mTotalRadiance.set(x, y, radTotal);
            Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
            mRenderFramebuffer->setPixel(x, y, color);
        }
    }
}
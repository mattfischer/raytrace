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
            startInitialSampleJob();
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

        void Renderer::startInitialSampleJob()
        {
            std::unique_ptr<Executor::Job> job = 
                std::make_unique<RasterJob>(
                    mSettings.width,
                    mSettings.height,
                    1,
                    [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height()); },
                    [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                        {
                            initialSamplePixel(x, y, mCurrentSample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                        },
                    [&]() { startDirectIlluminateJob(); }
                );

            mExecutor.runJob(std::move(job));
        }

        void Renderer::startDirectIlluminateJob()
        {
            std::unique_ptr<Executor::Job> job = 
                std::make_unique<RasterJob>(
                    mSettings.width,
                    mSettings.height,
                    1,
                    [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height()); },
                    [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                        {
                            directIlluminatePixel(x, y, mCurrentSample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                        },
                    [&]() { startIndirectIlluminateJob(); }
                );

            mExecutor.runJob(std::move(job));
        }

        void Renderer::startIndirectIlluminateJob()
        {
            std::unique_ptr<Executor::Job> job = 
                std::make_unique<RasterJob>(
                    mSettings.width,
                    mSettings.height,
                    1,
                    [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height()); },
                    [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                        {
                            indirectIlluminatePixel(x, y, mCurrentSample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                        },
                    [&]() 
                        {
                            mCurrentSample++;
                            if(mCurrentSample < mSettings.minSamples) {
                                startInitialSampleJob();
                            } else {
                                auto endTime = std::chrono::steady_clock::now();
                                std::chrono::duration<double> duration = endTime - mStartTime;
                                mListener->onRendererDone(duration.count());
                            }
                        }
                );

            mExecutor.runJob(std::move(job));
        }

        void Renderer::initialSamplePixel(int x, int y, int sample, Math::Sampler::Base &sampler)
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
        
                        Reservoir<DirectSample> &reservoir = mDirectReservoirs.at(x, y);
                        reservoir.sample.point = pnt2;
                        reservoir.sample.radiance = rad2;
                        reservoir.sample.normal = nrm2;
                        reservoir.sample.primitive = &light;

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
                  
                        Reservoir<IndirectSample> &reservoir = mIndirectReservoirs.at(x, y);
                        reservoir.sample.dirIn = dirIn;
                        reservoir.sample.indirectRadiance = (rad2 - isect2.primitive().surface().radiance()); 
                        float q = reservoir.sample.indirectRadiance.magnitude();
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

        void Renderer::directIlluminatePixel(int x, int y, int sample, Math::Sampler::Base &sampler)
        {
            Math::Radiance radDirect;
            PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
            const Math::Normal &nrmFacing = primaryHit.isect.facingNormal(); 
            const Object::Surface &surface = primaryHit.isect.primitive().surface();

            const int N = 1;
            DirectSample samples[N];
            float W = 0;
            int m = 0;
            const int R = 30;
            for(int i=0; i<30; i++) {
                Math::Point2D s = sampler.getValue2D();
                s = s * R * 2 + Math::Point2D(-R, -R);
                int sx = (int)std::floor(s.u() + x);
                int sy = (int)std::floor(s.v() + y);
                if(sx < 0 || sy < 0 || sx >= mSettings.width || sy >= mSettings.height) {
                    continue;
                }
                Reservoir<DirectSample> &reservoir = mDirectReservoirs.at(sx, sy);
                if(reservoir.weight == 0) {
                    continue;
                }

                W += reservoir.weight;
                if(m < N) {
                    samples[m] = reservoir.sample;
                } else if(sampler.getValue() < reservoir.weight / W) {
                    int n = (int)std::floor(sampler.getValue() * N);
                    samples[n] = reservoir.sample;
                }

                m++;
            }
            
            int n = std::min(m, N);
            for(int i=0; i<n; i++) {
                Math::Point pntOffset = primaryHit.isect.point() + Math::Vector(nrmFacing) * 0.01f;

                Math::Vector dirIn = samples[i].point - pntOffset;
                float d = dirIn.magnitude();
                dirIn = dirIn / d;
                float dot2 = std::abs(dirIn * samples[i].normal);

                float dot = dirIn * nrmFacing;
                
                if(dot > 0 && W > 0) {
                    Math::Ray ray(pntOffset, dirIn);
                    Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                    Object::Intersection isect2 = mScene.intersect(beam);

                    if (isect2.valid() && &(isect2.primitive()) == samples[i].primitive) {
                        Math::Radiance irad = samples[i].radiance * dot2 * dot / (d * d);
                        Math::Radiance rad = irad * surface.reflected(primaryHit.isect, dirIn);
                        float q = rad.magnitude();
                        radDirect += rad * W / (q * m * n);
                    }
                }
            }

            Math::Radiance radTotal = mTotalRadiance.get(x, y) + radDirect;
            mTotalRadiance.set(x, y, radTotal);
            Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
            mRenderFramebuffer->setPixel(x, y, color);
        }

        void Renderer::indirectIlluminatePixel(int x, int y, int sample, Math::Sampler::Base &sampler)
        {
            Math::Radiance radIndirect;
            PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
            const Math::Normal &nrmFacing = primaryHit.isect.facingNormal(); 
            const Object::Surface &surface = primaryHit.isect.primitive().surface();

            const int N = 10;
            IndirectSample samples[N];
            float W = 0;
            int m = 0;
            const int R = 30;
            for(int i=0; i<30; i++) {
                Math::Point2D s = sampler.getValue2D();
                s = s * R * 2 + Math::Point2D(-R, -R);
                int sx = (int)std::floor(s.u() + x);
                int sy = (int)std::floor(s.v() + y);
                if(sx < 0 || sy < 0 || sx >= mSettings.width || sy >= mSettings.height) {
                    continue;
                }
                Reservoir<IndirectSample> &reservoir = mIndirectReservoirs.at(sx, sy);
                if(reservoir.weight == 0) {
                    continue;
                }

                W += reservoir.weight;
                if(m < N) {
                    samples[m] = reservoir.sample;
                } else if(sampler.getValue() < reservoir.weight / W) {
                    int n = (int)std::floor(sampler.getValue() * N);
                    samples[n] = reservoir.sample;
                }

                m++;
            }
            
            int n = std::min(m, N);
            for(int i=0; i<n; i++) {
                float q = samples[i].indirectRadiance.magnitude();
                float dot = samples[i].dirIn * nrmFacing;
                if(dot > 0 && W > 0) {
                    Math::Color reflected = surface.reflected(primaryHit.isect, samples[i].dirIn);
                    radIndirect += samples[i].indirectRadiance * dot * reflected * W / (q * m * n);
                }
            }

            Math::Radiance radTotal = mTotalRadiance.get(x, y) + radIndirect;
            mTotalRadiance.set(x, y, radTotal);
            Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
            mRenderFramebuffer->setPixel(x, y, color);
        }
    }
}
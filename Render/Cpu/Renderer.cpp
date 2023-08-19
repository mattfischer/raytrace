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

            const Math::Normal &nrmFacing = isect.facingNormal(); 
            const Object::Surface &surface = isect.primitive().surface();
            Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

            Reservoir<DirectSample> &resDirect = mDirectReservoirs.at(x, y);
            resDirect.clear();
   
            Math::Radiance radEmitted;
            if (isect.valid()) {   
                for(int i=0; i<10; i++) {
                    int lightIndex = (int)std::floor(sampler.getValue() * mScene.areaLights().size());
                    const Object::Primitive &light = mScene.areaLights()[lightIndex];
                    const Math::Radiance &rad2 = light.surface().radiance();

                    Math::Point pnt2;
                    Math::Normal nrm2;
                    float pdf2;

                    if(light.shape().sample(sampler, pnt2, nrm2, pdf2)) {
                        Math::Vector dirIn = pnt2 - pntOffset;
                        float d = dirIn.magnitude();
                        dirIn = dirIn / d;
                        float dot2 = std::abs(dirIn * nrm2);

                        float dot = dirIn * nrmFacing;
                        if(dot > 0) {
                            Math::Radiance irad = rad2 * dot2 * dot / (d * d);
                            Math::Radiance radDirect = irad * surface.reflected(isect, dirIn);
                            float q = radDirect.magnitude();
                            float weight = q / pdf2;
                            
                            DirectSample sample;
                            sample.point = pnt2;
                            sample.radiance = rad2;
                            sample.normal = nrm2;
                            sample.primitive = &light;
                            
                            resDirect.update(sample, weight, q, sampler);
                        }
                    }
                }
                resDirect.W = resDirect.weight / (resDirect.q * resDirect.M);

                Reservoir<IndirectSample> &resIndirect = mIndirectReservoirs.at(x, y);           
                resIndirect.clear();

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
                  
                        IndirectSample sample;
                        sample.point = isect2.point();
                        sample.indirectRadiance = (rad2 - isect2.primitive().surface().radiance()); 
                        float q = sample.indirectRadiance.magnitude();
                        float weight = q / pdf;
                        resIndirect.update(sample, weight, q, sampler);
                        resIndirect.W = resIndirect.weight / (resIndirect.q * resIndirect.M);
                    }
                }

                radEmitted = isect.primitive().surface().radiance();
            } else {
                radEmitted = mScene.skyRadiance();
            }
    
            addRadiance(x, y, sample, radEmitted);
        }

        void Renderer::directIlluminatePixel(int x, int y, int sample, Math::Sampler::Base &sampler)
        {
            Math::Radiance radDirect;
            PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
            const Math::Normal &nrmFacing = primaryHit.isect.facingNormal(); 
            const Object::Surface &surface = primaryHit.isect.primitive().surface();
            Math::Point pntOffset = primaryHit.isect.point() + Math::Vector(nrmFacing) * 0.01f;

            Reservoir<DirectSample> res;
            res.clear();

            const int R = 30;
            for(int i=0; i<30; i++) {
                Math::Point2D s = sampler.getValue2D();
                s = s * R * 2 + Math::Point2D(-R, -R);
                int sx = (int)std::floor(s.u() + x);
                int sy = (int)std::floor(s.v() + y);
                if(sx < 0 || sy < 0 || sx >= mSettings.width || sy >= mSettings.height) {
                    continue;
                }
                Reservoir<DirectSample> &resCandidate = mDirectReservoirs.at(sx, sy);
                if(resCandidate.weight == 0) {
                    continue;
                }

                Math::Vector dirIn = resCandidate.sample.point - pntOffset;
                float d = dirIn.magnitude();
                dirIn = dirIn / d;
                float dot = dirIn * nrmFacing;
                float dot2 = std::abs(dirIn * resCandidate.sample.normal);
                
                float q = 0;
                if(dot > 0) {
                    Math::Radiance irad = resCandidate.sample.radiance * dot2 * dot / (d * d);
                    Math::Radiance rad = irad * surface.reflected(primaryHit.isect, dirIn);
                    q = rad.magnitude();
                }

                res.merge(resCandidate, q, sampler);
            }            
            res.W = res.weight / (res.q * res.M);

            if(res.W > 0) {
                Math::Vector dirIn = res.sample.point - pntOffset;
                float d = dirIn.magnitude();
                dirIn = dirIn / d;
                float dot = dirIn * nrmFacing;
                float dot2 = std::abs(dirIn * res.sample.normal);

                if(dot > 0) {
                    Math::Ray ray(pntOffset, dirIn);
                    Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                    Object::Intersection isect2 = mScene.intersect(beam);

                    if (isect2.valid() && &(isect2.primitive()) == res.sample.primitive) {
                        Math::Radiance irad = res.sample.radiance * dot2 * dot / (d * d);
                        Math::Radiance rad = irad * surface.reflected(primaryHit.isect, dirIn);
                        radDirect = rad * res.W;
                    }
                }
            }

            addRadiance(x, y, sample, radDirect);
        }

        void Renderer::indirectIlluminatePixel(int x, int y, int sample, Math::Sampler::Base &sampler)
        {
            Math::Radiance radIndirect;
            PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
            const Math::Normal &nrmFacing = primaryHit.isect.facingNormal(); 
            const Object::Surface &surface = primaryHit.isect.primitive().surface();

            const int N = 10;
            Reservoir<IndirectSample> res[N];
            for(int i=0; i<N; i++) {
                res[i].clear();
            }

            const int R = 30;
            for(int i=0; i<30; i++) {
                Math::Point2D s = sampler.getValue2D();
                s = s * R * 2 + Math::Point2D(-R, -R);
                int sx = (int)std::floor(s.u() + x);
                int sy = (int)std::floor(s.v() + y);
                if(sx < 0 || sy < 0 || sx >= mSettings.width || sy >= mSettings.height) {
                    continue;
                }
                Reservoir<IndirectSample> &resCandidate = mIndirectReservoirs.at(sx, sy);
                if(resCandidate.weight == 0) {
                    continue;
                }

                float q = resCandidate.q;
                for(int i=0; i<N; i++) {
                    res[i].merge(resCandidate, q, sampler);
                }
            }
            
            for(int i=0; i<N; i++) {
                res[i].W = res[i].weight / (res[i].q * res[i].M);

                if(res[i].W > 0) {
                    float q = res[i].sample.indirectRadiance.magnitude();
                    Math::Vector dirIn = res[i].sample.point - primaryHit.isect.point();
                    float d = dirIn.magnitude();
                    dirIn = dirIn / d;
                    float dot = dirIn * nrmFacing;
                    
                    if(dot > 0) {
                        Math::Color reflected = surface.reflected(primaryHit.isect, dirIn);
                        radIndirect += res[i].sample.indirectRadiance * dot * reflected * res[i].W;
                    }
                }
            }

            addRadiance(x, y, sample, radIndirect / N);
        }

        void Renderer::addRadiance(int x, int y, int sample, const Math::Radiance &radiance)
        {
            Math::Radiance radTotal = mTotalRadiance.get(x, y) + radiance;
            mTotalRadiance.set(x, y, radTotal);
            Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
            mRenderFramebuffer->setPixel(x, y, color);
        }
    }
}
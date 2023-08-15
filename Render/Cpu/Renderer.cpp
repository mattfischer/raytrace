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
        , mReservoirs(settings.width, settings.height)
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

            mReservoirs.at(x, y).weight = 0;
    
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
                        Math::Ray ray(pntOffset, dirIn);
                        Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                        Object::Intersection isect2 = mScene.intersect(beam);

                        if (isect2.valid() && &(isect2.primitive()) == &light) {
                            float pdfBrdf = surface.pdf(isect, dirIn) * dot2 / (d * d);
                            misWeightLight = pdf2 * pdf2 / (pdf2 * pdf2 + pdfBrdf * pdfBrdf);

                            Math::Radiance irad = rad2 * dot2 * dot / (d * d);
                            radDirectLight = irad * surface.reflected(isect, dirIn) / pdf2;                            
                        }
                    }
                }

                Math::Vector dirIn;
                float pdf;
                bool pdfDelta;
                Math::Color reflected = surface.sample(isect, sampler, dirIn, pdf, pdfDelta);
                float reverse = (dirIn * nrmFacing > 0) ? 1.0f : -1.0f;
                float dot = dirIn * nrmFacing * reverse;

                Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f * reverse;

                Math::Radiance radDirectBrdf;
                float misWeightBrdf = 0;
                if(dot > 0) {
                    Math::Ray reflectRay(pntOffset, dirIn);
                    Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
                    Object::Intersection isect2 = mScene.intersect(beam);

                    if (isect2.valid()) {
                        Math::Radiance rad2 = mIndirectLighter->light(isect2, sampler);

                        float dot2 = -isect2.facingNormal() * dirIn;
                        float pdfArea = pdf * dot2 / (isect2.distance() * isect2.distance());
                        float pdfLight = isect2.primitive().shape().samplePdf(isect2.point());
                        misWeightBrdf = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
                        radDirectBrdf = isect2.primitive().surface().radiance() * dot * reflected / pdf;;
                    
                        Reservoir &reservoir = mReservoirs.at(x, y);
                        reservoir.dirIn = dirIn;
                        reservoir.beam = isect.beam();
                        reservoir.isect = Object::Intersection(mScene, isect.primitive(), reservoir.beam, isect.shapeIntersection());
                        reservoir.indirectRadiance = (rad2 - isect2.primitive().surface().radiance()); 
                        float q = reservoir.indirectRadiance.magnitude();
                        reservoir.weight = q / pdf;
                    }
                }
                Math::Radiance radDirect = radDirectLight * misWeightLight + radDirectBrdf * misWeightBrdf;

                rad = radEmitted + radDirect;
            } else {
                rad = mScene.skyRadiance();
            }
    
            Math::Radiance radTotal = mTotalRadiance.get(x, y) + rad;
            mTotalRadiance.set(x, y, radTotal);
            Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
            mRenderFramebuffer->setPixel(x, y, color);
        }

        void Renderer::sampleIndirectPixel(int x, int y, int sample, Math::Sampler::Base &sampler)
        {
            Math::Radiance radIndirect;
            Reservoir &reservoir = mReservoirs.at(x, y);                
            if(reservoir.weight > 0) {
                float q = reservoir.indirectRadiance.magnitude();

                const Math::Normal &nrmFacing = reservoir.isect.facingNormal(); 
                const Object::Surface &surface = reservoir.isect.primitive().surface();
   
                float reverse = (reservoir.dirIn * nrmFacing > 0) ? 1.0f : -1.0f;
                float dot = reservoir.dirIn * nrmFacing * reverse;
                Math::Color reflected = surface.reflected(reservoir.isect, reservoir.dirIn);
                radIndirect = reservoir.indirectRadiance * dot * reflected * reservoir.weight / q;
            }

            Math::Radiance radTotal = mTotalRadiance.get(x, y) + radIndirect;
            mTotalRadiance.set(x, y, radTotal);
            Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
            mRenderFramebuffer->setPixel(x, y, color);
        }
    }
}
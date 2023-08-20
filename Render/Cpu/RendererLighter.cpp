#include "Render/Cpu/RendererLighter.hpp"
#include "Render/Cpu/RasterJob.hpp"

#include "Math/Sampler/Halton.hpp"

#include <atomic>
#include <mutex>

namespace Render {
    namespace Cpu {
        struct ThreadLocal : public Executor::Job::ThreadLocal {
            Math::Sampler::Halton sampler;

            ThreadLocal(int width, int height) : sampler(width, height) {}
        };
            
        RendererLighter::RendererLighter(const Object::Scene &scene, const Settings &settings, std::unique_ptr<Lighter::Base> lighter)
        : mScene(scene)
        , mSettings(settings)
        , mLighter(std::move(lighter))
        , mTotalRadiance(settings.width, settings.height)
        {
            mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);

            if(mLighter) {
                std::vector<std::unique_ptr<Render::Cpu::Executor::Job>> prerenderJobs = mLighter->createPrerenderJobs(scene, *mRenderFramebuffer);
                for (std::unique_ptr<Render::Cpu::Executor::Job> &job : prerenderJobs) {
                    mJobs.push_back(std::move(job));
                }
            }

            std::unique_ptr<Executor::Job> job = 
                std::make_unique<RasterJob>(
                    settings.width,
                    settings.height,
                    settings.samples,
                    [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height()); },
                    [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                        {
                            renderPixel(x, y, sample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                        }
                );
            mJobs.push_back(std::move(job));
        }

        void RendererLighter::start(Listener *listener)
        {
            mListener = listener;
            mStartTime = std::chrono::steady_clock::now();

            mCurrentJob = 0;
            mExecutor.runJob(std::move(mJobs[mCurrentJob]), [&]() { jobDone(); });
        }

        void RendererLighter::stop()
        {
            mExecutor.stop();
        }

        bool RendererLighter::running()
        {
            return mExecutor.running();
        }

        Render::Framebuffer &RendererLighter::renderFramebuffer()
        {
            return *mRenderFramebuffer;
        }

        void RendererLighter::jobDone()
        {
            mCurrentJob++;
            if(mCurrentJob < mJobs.size()) {
                mExecutor.runJob(std::move(mJobs[mCurrentJob]), [&]() { jobDone(); });
            } else {
                auto endTime = std::chrono::steady_clock::now();
                std::chrono::duration<double> duration = endTime - mStartTime;
                mListener->onRendererDone(duration.count());
            }
        }

        void RendererLighter::renderPixel(int x, int y, int sample, Math::Sampler::Base &sampler)
        {
            Math::Bivector dv;
            sampler.startSample(x, y, sample);
            Math::Point2D imagePoint = Math::Point2D((float)x, (float)y) + sampler.getValue2D();
            Math::Point2D aperturePoint = sampler.getValue2D();
            Math::Beam beam = mScene.camera().createPixelBeam(imagePoint, mSettings.width, mSettings.height, aperturePoint);
            Object::Intersection isect = mScene.intersect(beam);

            Math::Color color;
            if(mLighter) {
                Math::Radiance rad;
                if (isect.valid()) {
                    rad = mLighter->light(isect, sampler);
                } else {
                    rad = mScene.skyRadiance();
                }
        
                Math::Radiance radTotal = mTotalRadiance.get(x, y) + rad;
                mTotalRadiance.set(x, y, radTotal);
                Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
                mRenderFramebuffer->setPixel(x, y, color);
            } else {
                if(isect.valid()) {
                    Math::Color color = isect.albedo();
                    mRenderFramebuffer->setPixel(x, y, color);
                }
            }
        }
    }
}
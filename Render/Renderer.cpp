#include "Render/Renderer.hpp"
#include "Render/RasterJob.hpp"

#include "Math/Sampler/Random.hpp"

#include <atomic>
#include <mutex>

namespace Render {
    struct ThreadLocal : public Executor::Job::ThreadLocal {
        Math::Sampler::Random sampler;
    };
        
    Renderer::Renderer(const Object::Scene &scene, const Settings &settings, std::unique_ptr<Lighter::Base> lighter)
    : mScene(scene)
    , mSettings(settings)
    , mLighter(std::move(lighter))
    , mTotalRadiance(settings.width, settings.height)
    {
        mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
        mSampleStatusFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);

        if(mLighter) {
            std::vector<std::unique_ptr<Render::Executor::Job>> prerenderJobs = mLighter->createPrerenderJobs(scene, *mRenderFramebuffer);
            for (std::unique_ptr<Render::Executor::Job> &job : prerenderJobs) {
                mExecutor.addJob(std::move(job));
            }
        }

        std::unique_ptr<Executor::Job> job = 
            std::make_unique<RasterJob>(
                settings.width,
                settings.height,
                settings.minSamples,
                [&]() { return std::make_unique<ThreadLocal>(); },
                [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                    {
                        renderPixel(x, y, sample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                    }
            );
        mExecutor.addJob(std::move(job));
    }

    Render::Executor &Renderer::executor()
    {
        return mExecutor;
    }

    Render::Framebuffer &Renderer::renderFramebuffer()
    {
        return *mRenderFramebuffer;
    }

    Render::Framebuffer &Renderer::sampleStatusFramebuffer()
    {
        return *mSampleStatusFramebuffer;
    }

    void Renderer::renderPixel(int x, int y, int sample, Math::Sampler::Base &sampler)
    {
        Math::Bivector dv;
        sampler.startSample();
        Math::Point2D imagePoint = Math::Point2D((float)x, (float)y) + sampler.getValue2D();
        Math::Point2D aperturePoint = sampler.getValue2D();
        Math::Beam beam = mScene.camera().createPixelBeam(imagePoint, mSettings.width, mSettings.height, aperturePoint);
        Object::Intersection isect = mScene.intersect(beam);

        Object::Color color;
        if(mLighter) {
            Object::Radiance rad;
            if (isect.valid()) {
                rad = mLighter->light(isect, sampler);
            } else {
                rad = mScene.skyRadiance();
            }
    
            Object::Radiance radTotal = mTotalRadiance.get(x, y) + rad;
            mTotalRadiance.set(x, y, radTotal);
            Object::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
            mRenderFramebuffer->setPixel(x, y, color);
        } else {
            if(isect.valid()) {
                Object::Color color = isect.primitive().surface().albedo(isect);
                mRenderFramebuffer->setPixel(x, y, color);
            }
        }
    }
}
#define NOMINMAX
#include "Render/RenderJob.hpp"
#include "Render/Engine.hpp"

#include "Object/Radiance.hpp"
#include "Math/Bivector.hpp"
#include "Render/Engine.hpp"

#include "Math/Sampler/Halton.hpp"

#include <algorithm>
#include <random>

namespace Render {
    const unsigned int MaxSamplesPerIteration = 100;

    RenderJob::RenderJob(const Object::Scene &scene, const Settings &settings, const Lighter::Base *lighter, Framebuffer &renderFramebuffer, Framebuffer &sampleStatusFramebuffer)
        : TileJob(renderFramebuffer)
        , mScene(scene)
        , mSettings(settings)
        , mLighter(lighter)
        , mSampleStatusFramebuffer(sampleStatusFramebuffer)
        , mPixelsDone(renderFramebuffer.width(), renderFramebuffer.height())
        , mTotalRadiance(renderFramebuffer.width(), renderFramebuffer.height())
        , mSamplerOffsets(renderFramebuffer.width(), renderFramebuffer.height())
    {
        std::uniform_int_distribution<unsigned int> dist(0, 256);
        std::default_random_engine engine;
        for (unsigned int x = 0; x < renderFramebuffer.width(); x++) {
            for (unsigned int y = 0; y < renderFramebuffer.height(); y++) {
                mSamplerOffsets.set(x, y, dist(engine));
                sampleStatusFramebuffer.setPixel(x, y, Object::Color(0.25f, 0.25f, 0.25f));
            }
        }
        mNeedRepeat = false;
        mNumSamplesCompleted = 0;
        mNumSamplesThisIteration = 1;
    }

    std::unique_ptr<Job::ThreadLocal> RenderJob::createThreadLocal()
    {
        std::unique_ptr<ThreadLocal> threadLocal = std::make_unique<ThreadLocal>();
        threadLocal->sampler = std::make_unique<Math::Sampler::Halton>(50);

        return threadLocal;
    }

    void RenderJob::renderPixel(unsigned int x, unsigned int y, Job::ThreadLocal &threadLocal)
    {
        if (mPixelsDone.get(x, y)) {
            return;
        }

        Math::Sampler::Base &sampler = *static_cast<ThreadLocal&>(threadLocal).sampler;

        Object::Color color;
        Object::Color totalColor;
        bool pixelDone = false;

        const unsigned int runLength = 10;
        Object::Color colors[runLength];
        unsigned int colorIdx = 0;
        sampler.startSequence(mSamplerOffsets.get(x, y) + mNumSamplesCompleted);
        for (unsigned int sample = 0; sample < mNumSamplesThisIteration; sample++) {
            Math::Bivector dv;
            sampler.startSample();
            Math::Point2D imagePoint = Math::Point2D((float)x, (float)y) + sampler.getValue2D();
            Math::Point2D aperturePoint = sampler.getValue2D();
            Math::Beam beam = mScene.camera().createPixelBeam(imagePoint, framebuffer().width(), framebuffer().height(), aperturePoint);
            Object::Intersection isect = mScene.intersect(beam);
            unsigned int numSamples = mNumSamplesCompleted + sample + 1;

            if(mLighter) {
                Object::Radiance rad;
                if (isect.valid()) {
                    rad = mLighter->light(isect, sampler);
                } else {
                    rad = mScene.skyRadiance();
                }

                Object::Radiance radTotal = mTotalRadiance.get(x, y) + rad;
                mTotalRadiance.set(x, y, radTotal);
                color = Engine::toneMap(radTotal / static_cast<float>(numSamples));
            } else {
                if(isect.valid()) {
                    totalColor += isect.primitive().surface().albedo(isect);
                }
                color = totalColor / (sample + 1.0f);
            }

            colors[colorIdx] = color;
            colorIdx = (colorIdx + 1) % runLength;

            if (numSamples >= mSettings.maxSamples) {
                pixelDone = true;
                break;
            }

            if (numSamples > mSettings.minSamples) {
                float variance = 0;
                int numVarianceSamples = std::min(runLength, numSamples);
                for (int i = 0; i < numVarianceSamples; i++) {
                    variance += (colors[i] - color).magnitude2() / numVarianceSamples;
                }
                if (variance < mSettings.sampleThreshold * mSettings.sampleThreshold) {
                    pixelDone = true;
                    break;
                }
            }
        }

        framebuffer().setPixel(x, y, color);

        if (pixelDone) {
            mPixelsDone.set(x, y, true);
            float m = 3 * mNumSamplesCompleted / (mNumSamplesCompleted + 9.0f * mSettings.minSamples);
            float r = std::min(m, 1.0f);
            float g = std::max(std::min(m - 1, 1.0f), 0.0f);
            float b = std::max(m - 2.0f, 0.0f);
            mSampleStatusFramebuffer.setPixel(x, y, Object::Color(r, g, b));
        }
        else {
            mNeedRepeat = true;
        }
    }

    bool RenderJob::needRepeat()
    {
        bool needRepeat = mNeedRepeat;
        if (mNeedRepeat) {
            mNeedRepeat = false;
            mNumSamplesCompleted += mNumSamplesThisIteration;
            mNumSamplesThisIteration *= 2;
            mNumSamplesThisIteration = std::min(mNumSamplesThisIteration, MaxSamplesPerIteration);
        }

        return needRepeat;
    }
}

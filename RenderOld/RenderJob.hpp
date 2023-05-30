#ifndef RENDER_RENDER_JOB_HPP
#define RENDER_RENDER_JOB_HPP

#include "Render/TileJob.hpp"
#include "Render/Settings.hpp"
#include "Render/Raster.hpp"

#include "Object/Scene.hpp"

#include "Render/Lighter/Base.hpp"

#include "Math/Sampler/Base.hpp"

namespace Render {
    class RenderJob : public TileJob
    {
    public:
        RenderJob(const Object::Scene &scene, const Settings &settings, const Lighter::Base *lighter, Framebuffer &renderFramebuffer, Framebuffer &sampleStatusFramebuffer);

        std::unique_ptr<Job::ThreadLocal> createThreadLocal() override;

    private:
        struct ThreadLocal : public Job::ThreadLocal
        {
            std::unique_ptr<Math::Sampler::Base> sampler;
        };

        void renderPixel(unsigned int x, unsigned int y, Job::ThreadLocal &threadLocal) override;
        bool needRepeat() override;

        const Object::Scene &mScene;
        const Settings &mSettings;
        const Lighter::Base *mLighter;

        Framebuffer &mSampleStatusFramebuffer;
        unsigned int mNumSamplesThisIteration;
        Raster<bool> mPixelsDone;
        Raster<Object::Radiance> mTotalRadiance;
        Raster<unsigned int> mSamplerOffsets;
        unsigned int mNumSamplesCompleted;
        bool mNeedRepeat;
    };
}
#endif

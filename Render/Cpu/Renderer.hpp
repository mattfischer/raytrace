#ifndef RENDER_CPU_RENDERER_HPP
#define RENDER_CPU_RENDERER_HPP

#include "Render/Renderer.hpp"

#include "Render/Cpu/Executor.hpp"
#include "Render/Framebuffer.hpp"
#include "Render/Raster.hpp"

#include "Render/Cpu/Lighter/Base.hpp"

#include "Object/Scene.hpp"

#include <memory>
#include <chrono>

namespace Render {
    namespace Cpu {
        class Renderer : public Render::Renderer {
        public:
            struct Settings
            {
                unsigned int width;
                unsigned int height;
                unsigned int minSamples;
                unsigned int maxSamples;
                float sampleThreshold;
            };
            Renderer(const Object::Scene &scene, const Settings &settings, std::unique_ptr<Lighter::Base> lighter);

            void start(Listener *listener) override;
            void stop() override;
            bool running() override;

            Render::Framebuffer &renderFramebuffer() override;
            Render::Framebuffer &sampleStatusFramebuffer() override;

        private:
            void startRenderJob();
            void startSampleIndirectJob();
            void renderJobDone();
            void sampleIndirectJobDone();
            void renderPixel(int x, int y, int sample, Math::Sampler::Base &sampler);
            void sampleIndirectPixel(int x, int y, int sample, Math::Sampler::Base &sampler);

            Executor mExecutor;
            Listener *mListener;
            std::unique_ptr<Executor::Job> mRenderJob;
            int mCurrentSample;
            std::chrono::time_point<std::chrono::steady_clock> mStartTime;

            const Object::Scene &mScene;
            Settings mSettings;
            std::unique_ptr<Render::Framebuffer> mRenderFramebuffer;
            std::unique_ptr<Render::Framebuffer> mSampleStatusFramebuffer;

            std::unique_ptr<Render::Cpu::Lighter::Base> mIndirectLighter;

            struct Reservoir {
                Object::Intersection isect;
                Math::Beam beam;
                Math::Vector dirIn;
                Math::Radiance indirectRadiance;
                float weight;
            };
            Render::Raster<Reservoir> mReservoirs;

            Render::Raster<Math::Radiance> mTotalRadiance;
        };
    }
}
#endif
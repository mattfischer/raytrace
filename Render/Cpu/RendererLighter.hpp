#ifndef RENDER_CPU_RENDERER_LIGHTER_HPP
#define RENDER_CPU_RENDERER_LIGHTER_HPP

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
        class RendererLighter : public Render::Renderer {
        public:
            struct Settings
            {
                unsigned int width;
                unsigned int height;
                unsigned int samples;
            };
            RendererLighter(const Object::Scene &scene, const Settings &settings, std::unique_ptr<Lighter::Base> lighter);

            void start(Listener *listener) override;
            void stop() override;
            bool running() override;

            Render::Framebuffer &renderFramebuffer() override;

        private:
            void jobDone();
            void renderPixel(int x, int y, int sample, Math::Sampler::Base &sampler);

            Executor mExecutor;
            Listener *mListener;
            std::vector<std::unique_ptr<Executor::Job>> mJobs;
            int mCurrentJob;
            std::chrono::time_point<std::chrono::steady_clock> mStartTime;

            const Object::Scene &mScene;
            Settings mSettings;
            std::unique_ptr<Render::Framebuffer> mRenderFramebuffer;

            std::unique_ptr<Render::Cpu::Lighter::Base> mLighter;

            Render::Raster<Math::Radiance> mTotalRadiance;
        };
    }
}
#endif
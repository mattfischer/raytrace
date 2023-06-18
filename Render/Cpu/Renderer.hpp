#ifndef RENDER_CPU_RENDERER_HPP
#define RENDER_CPU_RENDERER_HPP

#include "Render/Renderer.hpp"

#include "Render/Executor.hpp"
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

            std::unique_ptr<Render::Cpu::Lighter::Base> mLighter;

            Render::Raster<Object::Radiance> mTotalRadiance;
        };
    }
}
#endif
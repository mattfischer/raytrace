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
            void startInitialSampleJob();
            void startDirectIlluminateJob();
            void startIndirectIlluminateJob();
            void initialSamplePixel(int x, int y, int sample, Math::Sampler::Base &sampler);
            void directIlluminatePixel(int x, int y, int sample, Math::Sampler::Base &sampler);
            void indirectIlluminatePixel(int x, int y, int sample, Math::Sampler::Base &sampler);

            Executor mExecutor;
            Listener *mListener;
            int mCurrentSample;
            std::chrono::time_point<std::chrono::steady_clock> mStartTime;

            const Object::Scene &mScene;
            Settings mSettings;
            std::unique_ptr<Render::Framebuffer> mRenderFramebuffer;
            std::unique_ptr<Render::Framebuffer> mSampleStatusFramebuffer;

            std::unique_ptr<Render::Cpu::Lighter::Base> mIndirectLighter;

            template<typename T> struct Reservoir {
                T sample;
                float weight;
                float W;
                float M;
                float q;
            };

            struct DirectSample {
                Math::Radiance radiance;
                Math::Point point;
                Math::Normal normal;
                const Object::Primitive *primitive;
            };

            Render::Raster<Reservoir<DirectSample>> mDirectReservoirs;

            struct IndirectSample {
                Math::Point point;
                Math::Radiance indirectRadiance;
            };
            Render::Raster<Reservoir<IndirectSample>> mIndirectReservoirs;

            struct PrimaryHit {
                Object::Intersection isect;
                Math::Beam beam;
            };
            Render::Raster<PrimaryHit> mPrimaryHits;

            Render::Raster<Math::Radiance> mTotalRadiance;
        };
    }
}
#endif
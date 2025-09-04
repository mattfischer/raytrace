#ifndef RENDER_CPU_RENDERER_RESTIR_HPP
#define RENDER_CPU_RENDERER_RESTIR_HPP

#include "Render/Renderer.hpp"

#include "Render/Cpu/Executor.hpp"
#include "Render/Framebuffer.hpp"
#include "Render/Raster.hpp"

#include "Render/Cpu/Impl/Lighter/UniPath.hpp"
#include "Math/Impl/Sampler/Halton.hpp"

#include "Object/Scene.hpp"

#include <memory>
#include <chrono>

namespace Render::Cpu {
    class RendererReSTIR : public Render::Renderer {
    public:
        struct Settings
        {
            unsigned int width;
            unsigned int height;
            unsigned int samples;
            unsigned int indirectSamples;
            unsigned int radius;
            unsigned int candidates;
        };
        RendererReSTIR(const Object::Scene &scene, const Settings &settings);

        void start(Listener *listener) override;
        void stop() override;
        bool running() override;

        Render::Framebuffer &renderFramebuffer() override;

    private:
        template<typename T> struct Reservoir {
            T sample;
            float W;
            int M;
            float q;

            void clear()
            {
                W = 0;
                M = 0;
                q = 0;
            }

            void addSample(const T &sampleNew, float qNew, float pdfNew, Math::Sampler &sampler)
            {
                combine(sampleNew, qNew, 1.0f / pdfNew, 1, 1.0f, sampler);
            }

            void addReservoir(const Reservoir<T> &resNew, float qNew, float J, Math::Sampler &sampler)
            {
                combine(resNew.sample, qNew, resNew.W, resNew.M, J, sampler);
            }

        private:
            void combine(const T &sampleNew, float qNew, float WNew, int MNew, float J, Math::Sampler &sampler)
            {
                float m0 = (float)M / (float)(M + MNew);
                float m1 = (float)MNew / (float)(M + MNew);

                float w0 = m0 * q * W;
                float w1 = m1 * qNew * WNew * J;
                float wSum = w0 + w1;

                if(w0 == 0 || sampler.getValue() < w1 / wSum) {
                    sample = sampleNew;
                    q = qNew;
                }

                M += MNew;
                W = wSum / q;
            }
        };

        struct DirectSample {
            Math::Radiance radiance;
            Math::Point point;
            Math::Normal normal;
            const Object::Primitive *primitive;
        };

        struct IndirectSample {
            Math::Point point;
            Math::Normal normal;
            Math::Radiance indirectRadiance;
        };

        void startInitialSampleJob();
        void startDirectIlluminateJob();
        void startIndirectIlluminateJob();
        void initialSamplePixel(int x, int y, int sample, Math::Sampler &sampler);
        void directIlluminatePixel(int x, int y, int sample, Math::Sampler &sampler);
        void indirectIlluminatePixel(int x, int y, int sample, Math::Sampler &sampler, Reservoir<IndirectSample> indirectSamples[]);

        void addRadiance(int x, int y, int sample, const Math::Radiance &radiance);

        Executor mExecutor;
        Listener *mListener;
        int mCurrentSample;
        std::chrono::time_point<std::chrono::steady_clock> mStartTime;

        const Object::Scene &mScene;
        Settings mSettings;
        std::unique_ptr<Render::Framebuffer> mRenderFramebuffer;

        std::unique_ptr<Render::Cpu::Lighter> mIndirectLighter;

        Render::Raster<Reservoir<DirectSample>> mDirectReservoirs;
        Render::Raster<Reservoir<IndirectSample>> mIndirectReservoirs;

        struct PrimaryHit {
            Object::Intersection isect;
            Math::Beam beam;
        };
        Render::Raster<PrimaryHit> mPrimaryHits;

        Render::Raster<Math::Radiance> mTotalRadiance;

        struct ThreadLocal : public Executor::Job::ThreadLocal {
            Math::Impl::Sampler::Halton sampler;
            std::vector<Reservoir<IndirectSample>> indirectSamples;

            ThreadLocal(int width, int height, int numIndirectSamples)
                : sampler(width, height)
                , indirectSamples(numIndirectSamples)
            {}
        };    
    };
}
#endif
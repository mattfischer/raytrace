#ifndef RENDER_RENDERER_HPP
#define RENDER_RENDERER_HPP

#include "Render/Executor.hpp"
#include "Render/Framebuffer.hpp"

namespace Render {
    class Renderer : public Executor::Listener{
    public:
        class Listener {
        public:
            virtual ~Listener() = default;
            virtual void onRendererDone(float totalTimeSeconds) = 0;
        };

        virtual ~Renderer() = default;

        void start(Listener *listener);
        void stop();
        bool running();

        Render::Framebuffer &renderFramebuffer();
        Render::Framebuffer &sampleStatusFramebuffer();

        void onExecutorDone(float totalTimeSeconds);

    protected:
        Render::Executor mExecutor;
        Listener *mListener;
        std::unique_ptr<Render::Framebuffer> mRenderFramebuffer;
        std::unique_ptr<Render::Framebuffer> mSampleStatusFramebuffer;
    };
}

#endif
#ifndef RENDER_RENDERER_HPP
#define RENDER_RENDERER_HPP

#include "Render/Executor.hpp"
#include "Render/Framebuffer.hpp"

namespace Render {
    class Renderer {
    public:
        class Listener {
        public:
            virtual ~Listener() = default;
            virtual void onRendererDone(float totalTimeSeconds) = 0;
        };

        virtual ~Renderer() = default;

        virtual void start(Listener *listener) = 0;
        virtual void stop() = 0;
        virtual bool running() = 0;

        Render::Framebuffer &renderFramebuffer();
        Render::Framebuffer &sampleStatusFramebuffer();

    protected:
        std::unique_ptr<Render::Framebuffer> mRenderFramebuffer;
        std::unique_ptr<Render::Framebuffer> mSampleStatusFramebuffer;
    };
}

#endif
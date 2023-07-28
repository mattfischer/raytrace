#ifndef RENDER_RENDERER_HPP
#define RENDER_RENDERER_HPP

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

        virtual Render::Framebuffer &renderFramebuffer() = 0;
        virtual Render::Framebuffer &sampleStatusFramebuffer() = 0;
    };
}

#endif
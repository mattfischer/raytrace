#ifndef RENDER_RENDERER_HPP
#define RENDER_RENDERER_HPP

#include "Render/Executor.hpp"
#include "Render/Framebuffer.hpp"

namespace Render {
    class Renderer {
    public:
        virtual ~Renderer() = default;

        Render::Executor &executor();
        Render::Framebuffer &renderFramebuffer();
        Render::Framebuffer &sampleStatusFramebuffer();

    protected:
        Render::Executor mExecutor;
        std::unique_ptr<Render::Framebuffer> mRenderFramebuffer;
        std::unique_ptr<Render::Framebuffer> mSampleStatusFramebuffer;
    };
}

#endif
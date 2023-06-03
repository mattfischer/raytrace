#include "Render/Renderer.hpp"

namespace Render {
    Render::Executor &Renderer::executor()
    {
        return mExecutor;
    }

    Render::Framebuffer &Renderer::renderFramebuffer()
    {
        return *mRenderFramebuffer;
    }

    Render::Framebuffer &Renderer::sampleStatusFramebuffer()
    {
        return *mSampleStatusFramebuffer;
    }
}
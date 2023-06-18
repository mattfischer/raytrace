#include "Render/Renderer.hpp"

namespace Render {
    Render::Framebuffer &Renderer::renderFramebuffer()
    {
        return *mRenderFramebuffer;
    }

    Render::Framebuffer &Renderer::sampleStatusFramebuffer()
    {
        return *mSampleStatusFramebuffer;
    }
}
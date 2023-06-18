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

    void Renderer::start(Listener *listener)
    {
        mListener = listener;

        mExecutor.start(this);
    }

    void Renderer::stop()
    {
        mExecutor.stop();
    }

    bool Renderer::running()
    {
        return mExecutor.running();
    }

    void Renderer::onExecutorDone(float totalTimeSeconds)
    {
        mListener->onRendererDone(totalTimeSeconds);
        mListener = nullptr;
    }
}
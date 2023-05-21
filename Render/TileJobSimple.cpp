#include "Render/TileJobSimple.hpp"

#include "Math/Sampler/Halton.hpp"

namespace Render {
    TileJobSimple::TileJobSimple(Framebuffer &framebuffer, std::function<void(unsigned int, unsigned int, Framebuffer&, Math::Sampler::Base&)> &&pixelFunc)
        : TileJob(framebuffer), mPixelFunc(std::move(pixelFunc))
    {
    }

    std::unique_ptr<Job::ThreadLocal> TileJobSimple::createThreadLocal()
    {
        std::unique_ptr<ThreadLocal> threadLocal = std::make_unique<ThreadLocal>();
        threadLocal->sampler = std::make_unique<Math::Sampler::Halton>(10);

        return threadLocal;
    }

    void TileJobSimple::renderPixel(unsigned int x, unsigned int y, Job::ThreadLocal &threadLocal)
    {
        Math::Sampler::Base &sampler = *static_cast<ThreadLocal&>(threadLocal).sampler;

        mPixelFunc(x, y, framebuffer(), sampler);
    }
}

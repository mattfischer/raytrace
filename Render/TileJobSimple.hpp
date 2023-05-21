#ifndef RENDER_TILE_JOB_SIMPLE_HPP
#define RENDER_TILE_JOB_SIMPLE_HPP

#include "Render/TileJob.hpp"
#include "Math/Sampler/Base.hpp"

namespace Render {
    class TileJobSimple : public TileJob
    {
    public:
        TileJobSimple(Framebuffer &framebuffer, std::function<void(unsigned int, unsigned int, Framebuffer&, Math::Sampler::Base&)> &&pixelFunc);

        virtual std::unique_ptr<Job::ThreadLocal> createThreadLocal();

    protected:
        virtual void renderPixel(unsigned int x, unsigned int y, Job::ThreadLocal &threadLocal);

    private:
        struct ThreadLocal : public Job::ThreadLocal
        {
            std::unique_ptr<Math::Sampler::Base> sampler;
        };

        std::function<void(unsigned int, unsigned int, Framebuffer &, Math::Sampler::Base&)> mPixelFunc;
    };
}
#endif

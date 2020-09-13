#include "Render/TileJobSimple.hpp"

namespace Render {
    TileJobSimple::TileJobSimple(Framebuffer &framebuffer, std::function<void(unsigned int, unsigned int, Framebuffer&, Sampler&)> &&pixelFunc)
		: TileJob(framebuffer), mPixelFunc(std::move(pixelFunc))
	{
	}

	std::unique_ptr<Job::ThreadLocal> TileJobSimple::createThreadLocal()
	{
		return std::make_unique<ThreadLocal>(10);
	}

    void TileJobSimple::renderPixel(unsigned int x, unsigned int y, Job::ThreadLocal &threadLocal)
	{
		Sampler &sampler = static_cast<ThreadLocal&>(threadLocal).sampler;

		mPixelFunc(x, y, framebuffer(), sampler);
	}
}

#ifndef RENDER_TILE_JOB_SIMPLE_HPP
#define RENDER_TILE_JOB_SIMPLE_HPP

#include "Render/TileJob.hpp"

namespace Render {
	class TileJobSimple : public TileJob
	{
	public:
		TileJobSimple(Framebuffer &framebuffer, std::function<void(int, int, Framebuffer&, Sampler&)> &&pixelFunc);

		virtual std::unique_ptr<Job::ThreadLocal> createThreadLocal();

	protected:
		virtual void renderPixel(int x, int y, Job::ThreadLocal &threadLocal);

	private:
		struct ThreadLocal : public Job::ThreadLocal
		{
			Sampler sampler;

			ThreadLocal(int samplerDimensions) : sampler(samplerDimensions) {}
		};

		std::function<void(int, int, Framebuffer &, Sampler&)> mPixelFunc;
	};
}
#endif
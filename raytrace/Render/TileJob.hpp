#ifndef RENDER_TILE_JOB_HPP
#define RENDER_TILE_JOB_HPP

#include "Render/Job.hpp"
#include "Render/Framebuffer.hpp"

namespace Render {
	class TileJob : public Job
	{
	public:
		TileJob(Framebuffer &framebuffer);

		std::unique_ptr<Job::Task> getNextTask();

	protected:
		virtual void renderPixel(int x, int y, Sampler &sampler) = 0;
		Framebuffer &framebuffer();

	private:
		Framebuffer &mFramebuffer;
		int mNextTile;
	};

	class TileJobSimple : public TileJob
	{
	public:
		TileJobSimple(Framebuffer &framebuffer, std::function<void(int, int, Framebuffer&, Sampler&)> &&pixelFunc);

	protected:
		virtual void renderPixel(int x, int y, Sampler &sampler);

	private:
		std::function<void(int, int, Framebuffer &, Sampler&)> mPixelFunc;
	};
}
#endif
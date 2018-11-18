#include "Render/TileJob.hpp"

#include <algorithm>

namespace Render {
	const int TILE_SIZE = 64;

	TileJob::TileJob(Framebuffer &framebuffer, std::function<void(int, int, Framebuffer&, Sampler&)> &&pixelFunc)
		: mFramebuffer(framebuffer), mPixelFunc(std::move(pixelFunc))
	{
		mNextTile = 0;
	}

	std::unique_ptr<Job::Task> TileJob::getNextTask()
	{
		std::unique_ptr<Job::Task> task;

		int widthInTiles = (mFramebuffer.width() + TILE_SIZE - 1) / TILE_SIZE;
		int heightInTiles = (mFramebuffer.height() + TILE_SIZE - 1) / TILE_SIZE;
		if (mNextTile < widthInTiles * heightInTiles) {
			int x = (mNextTile % widthInTiles) * TILE_SIZE;
			int y = (mNextTile / widthInTiles) * TILE_SIZE;
			int width = std::min(TILE_SIZE, mFramebuffer.width() - x);
			int height = std::min(TILE_SIZE, mFramebuffer.height() - y);

			auto func = [=](Sampler &sampler) {
				for (int j = y; j < y + height; j++) {
					for (int i = x; i < x + width; i++) {
						mPixelFunc(i, j, mFramebuffer, sampler);
					}
				}
			};

			task = std::make_unique<Job::Task>(std::move(func));
			mNextTile++;
		}

		return task;
	}
}
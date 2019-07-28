#include "Render/TileJob.hpp"

#include <algorithm>

namespace Render {
	const int TILE_SIZE = 64;

	TileJob::TileJob(Framebuffer &framebuffer)
		: mFramebuffer(framebuffer)
	{
		mNextTile = 0;
		mOutstandingJobs = 0;
		mDone = false;
		mWidthInTiles = (mFramebuffer.width() + TILE_SIZE - 1) / TILE_SIZE;
		mHeightInTiles = (mFramebuffer.height() + TILE_SIZE - 1) / TILE_SIZE;
	}

	std::unique_ptr<Job::Task> TileJob::getNextTask()
	{
		std::unique_lock<std::mutex> lock(mMutex);

		std::unique_ptr<Job::Task> task;

		while (!mDone) {
			if (mNextTile < mWidthInTiles * mHeightInTiles) {
				int x = (mNextTile % mWidthInTiles) * TILE_SIZE;
				int y = (mNextTile / mWidthInTiles) * TILE_SIZE;
				int width = std::min(TILE_SIZE, mFramebuffer.width() - x);
				int height = std::min(TILE_SIZE, mFramebuffer.height() - y);

				auto func = [=](Sampler &sampler) {
					for (int j = y; j < y + height; j++) {
						for (int i = x; i < x + width; i++) {
							renderPixel(i, j, sampler);
						}
					}
					jobDone();
				};

				task = std::make_unique<Job::Task>(std::move(func));
				mNextTile++;
				mOutstandingJobs++;
				break;
			}
			else {
				mCondVar.wait(lock);
			}
		}

		return task;
	}

	Framebuffer &TileJob::framebuffer()
	{
		return mFramebuffer;
	}

	void TileJob::jobDone()
	{
		std::unique_lock<std::mutex>(mMutex);

		mOutstandingJobs--;
		if (mOutstandingJobs == 0 && mNextTile >= mWidthInTiles * mHeightInTiles) {
			mNextTile = 0;
			mDone = frameDone();
			mCondVar.notify_all();
		}
	}

	TileJobSimple::TileJobSimple(Framebuffer &framebuffer, std::function<void(int, int, Framebuffer&, Sampler&)> &&pixelFunc)
		: TileJob(framebuffer), mPixelFunc(std::move(pixelFunc))
	{
	}

	void TileJobSimple::renderPixel(int x, int y, Sampler &sampler)
	{
		mPixelFunc(x, y, framebuffer(), sampler);
	}

	bool TileJobSimple::frameDone()
	{
		return true;
	}
}
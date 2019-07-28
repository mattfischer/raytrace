#include "Render/TileJob.hpp"

#include <algorithm>

namespace Render {
	const int TILE_SIZE = 64;

	TileJob::TileJob(Framebuffer &framebuffer)
		: mFramebuffer(framebuffer)
	{
		mNextTile = 0;
		mOutstandingTasks = 0;
		mStop = false;
		mWidthInTiles = (mFramebuffer.width() + TILE_SIZE - 1) / TILE_SIZE;
		mHeightInTiles = (mFramebuffer.height() + TILE_SIZE - 1) / TILE_SIZE;
	}

	std::unique_ptr<Job::Task> TileJob::getNextTask()
	{
		std::unique_lock<std::mutex> lock(mMutex);

		std::unique_ptr<Job::Task> task;

		while (!mStop) {
			if (mNextTile < mWidthInTiles * mHeightInTiles) {
				int x = (mNextTile % mWidthInTiles) * TILE_SIZE;
				int y = (mNextTile / mWidthInTiles) * TILE_SIZE;
				int width = std::min(TILE_SIZE, mFramebuffer.width() - x);
				int height = std::min(TILE_SIZE, mFramebuffer.height() - y);

				auto func = [=](Job::ThreadLocal &threadLocal) {
					for (int j = y; j < y + height; j++) {
						for (int i = x; i < x + width; i++) {
							renderPixel(i, j, threadLocal);
						}
					}
					taskDone();
				};

				task = std::make_unique<Job::Task>(std::move(func));
				mNextTile++;
				mOutstandingTasks++;
				break;
			}
			else {
				mCondVar.wait(lock);
			}
		}

		return task;
	}

	void TileJob::stop()
	{
		std::unique_lock<std::mutex>(mMutex);
		mStop = true;
		mCondVar.notify_all();
	}

	Framebuffer &TileJob::framebuffer()
	{
		return mFramebuffer;
	}

	void TileJob::taskDone()
	{
		std::unique_lock<std::mutex>(mMutex);

		mOutstandingTasks--;
		if (mOutstandingTasks == 0 && mNextTile >= mWidthInTiles * mHeightInTiles) {
			mNextTile = 0;
			mStop = !needRepeat();
			mCondVar.notify_all();
		}
	}

	bool TileJob::needRepeat()
	{
		return false;
	}
}
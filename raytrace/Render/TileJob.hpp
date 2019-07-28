#ifndef RENDER_TILE_JOB_HPP
#define RENDER_TILE_JOB_HPP

#include "Render/Job.hpp"
#include "Render/Framebuffer.hpp"

#include <mutex>

namespace Render {
	class TileJob : public Job
	{
	public:
		TileJob(Framebuffer &framebuffer);

		std::unique_ptr<Job::Task> getNextTask();

	protected:
		virtual void renderPixel(int x, int y, Job::ThreadLocal &threadLocal) = 0;
		virtual bool needRepeat();
		Framebuffer &framebuffer();

	private:
		void taskDone();

		Framebuffer &mFramebuffer;
		int mWidthInTiles;
		int mHeightInTiles;
		int mNextTile;
		bool mRepeat;
		int mOutstandingTasks;
		std::mutex mMutex;
		std::condition_variable mCondVar;
	};
}
#endif
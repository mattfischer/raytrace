#include "Render/Job.hpp"

namespace Render {
	void Job::setDoneHandler(std::function<void()> &&handler)
	{
		mDoneHandler = std::move(handler);
	}

	void Job::done()
	{
		if (mDoneHandler) {
			mDoneHandler();
		}
	}
}
#ifndef RENDER_JOB_HPP
#define RENDER_JOB_HPP

#include "Render/Sampler.hpp"

#include <memory>
#include <functional>

namespace Render {
	class Job {
	public:
		typedef std::function<void(Sampler &)> Task;

		virtual std::unique_ptr<Task> getNextTask() = 0;

		void setDoneHandler(std::function<void()> &&handler);
		void done();

	private:
		std::function<void()> mDoneHandler;
	};

}
#endif
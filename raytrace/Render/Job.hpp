#ifndef RENDER_JOB_HPP
#define RENDER_JOB_HPP

#include "Render/Sampler.hpp"

#include <memory>
#include <functional>

namespace Render {
	class Job {
	public:
		struct ThreadLocal {
		};
		typedef std::function<void(ThreadLocal &)> Task;

		virtual std::unique_ptr<Task> getNextTask() = 0;
		virtual std::unique_ptr<ThreadLocal> createThreadLocal() = 0;

		void setDoneHandler(std::function<void()> &&handler);
		void done();

	private:
		std::function<void()> mDoneHandler;
	};

}
#endif
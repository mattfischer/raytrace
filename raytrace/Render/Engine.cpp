#define NOMINMAX
#include "Render/Engine.hpp"

#include "Render/RenderJob.hpp"

#include <algorithm>
#include <memory>

namespace Render {
	Engine::Engine(const Object::Scene &scene)
		: mScene(scene)
	{
		mRendering = false;
		mFramebuffer = std::make_unique<Framebuffer>(0, 0);
		mStopThreads = false;

		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		int numThreads = sysinfo.dwNumberOfProcessors;

		mNumRunningThreads = 0;
		for (int i = 0; i < numThreads; i++) {
			mThreads.push_back(std::make_unique<std::thread>([=]() { runThread(); }));
		}
	}

	void Engine::stop()
	{
		{
			std::unique_lock<std::mutex> lock(mMutex);
			mStopThreads = true;
			mConditionVariable.notify_all();
		}

		for (std::unique_ptr<std::thread> &thread : mThreads) {
			thread->join();
		}
	}

	const Object::Scene &Engine::scene() const
	{
		return mScene;
	}

	void Engine::startRender(Listener *listener)
	{
		mListener = listener;
		mRendering = true;
		mListener->onRenderStatus("");

		mStartTime = GetTickCount();

		if (mSettings.lighting) {
			mLighter = std::make_unique<Lighter::Master>(mSettings.lighterSettings);
			std::vector<std::unique_ptr<Job>> prerenderJobs = mLighter->createPrerenderJobs(mScene, *mFramebuffer);
			for (std::unique_ptr<Job> &job : prerenderJobs) {
				addJob(std::move(job));
			}
		}

		std::unique_ptr<Job> renderJob = std::make_unique<RenderJob>(mScene, mSettings, *mLighter, *mFramebuffer);
		renderJob->setDoneHandler([=]() { renderDone(); } );
		addJob(std::move(renderJob));
	}

	bool Engine::rendering() const
	{
		return mRendering;
	}

	const Settings &Engine::settings() const
	{
		return mSettings;
	}

	void Engine::setSettings(const Settings &settings)
	{
		if (settings.width != mSettings.width || settings.height != mSettings.height) {
			mFramebuffer = std::make_unique<Framebuffer>(settings.width, settings.height);
		}
		mSettings = settings;
	}

	Framebuffer &Engine::framebuffer()
	{
		return *mFramebuffer;
	}

	Object::Color Engine::toneMap(const Object::Radiance &radiance)
	{
		float red = radiance.red() / (radiance.red() + 1);
		float green = radiance.green() / (radiance.green() + 1);
		float blue = radiance.blue() / (radiance.blue() + 1);

		return Object::Color(red, green, blue);
	}

	void Engine::addJob(std::unique_ptr<Job> job)
	{
		std::unique_lock<std::mutex> lock(mMutex);

		if (!mCurrentJob && mNumRunningThreads == 0) {
			mCurrentJob = std::move(job);
			mConditionVariable.notify_all();
		}
		else {
			mJobs.push_back(std::move(job));
		}
	}

	void Engine::runThread()
	{
		while (true) {
			std::unique_lock<std::mutex> lock(mMutex);
			if (mStopThreads) {
				return;
			}

			mNumRunningThreads++;

			bool block = false;

			if (mCurrentJob) {
				Sampler sampler(10);

				while (true) {
					if (mStopThreads) {
						return;
					}
					std::unique_ptr<Job::Task> task = mCurrentJob->getNextTask();

					if (!task) {
						block = true;
						break;
					}

					lock.unlock();
					(*task)(sampler);
					lock.lock();
				}
			}

			mNumRunningThreads--;

			if (mNumRunningThreads == 0) {
				if (mCurrentJob) {
					mCurrentJob->done();
					mCurrentJob.reset();
				}

				if (mJobs.size() > 0) {
					mCurrentJob = std::move(mJobs.front());
					mJobs.pop_front();
					mConditionVariable.notify_all();
					block = false;
				}
				else {
					block = true;
				}
			}

			if (block) {
				mConditionVariable.wait(lock);
			}
		}
	}

	void Engine::renderDone()
	{
		DWORD endTime = GetTickCount();
		char buf[256];
		float seconds = (endTime - mStartTime) / 1000.0f;
		int hours = seconds / 3600;
		seconds -= hours * 3600;
		int minutes = seconds / 60;
		seconds -= minutes * 60;
		if (hours > 0) {
			sprintf_s(buf, sizeof(buf), "Render time: %ih %im %is", hours, minutes, (int)seconds);
		}
		else if (minutes > 0) {
			sprintf_s(buf, sizeof(buf), "Render time: %im %is", minutes, (int)seconds);
		}
		else {
			sprintf_s(buf, sizeof(buf), "Render time: %.3fs", seconds);
		}
		mRendering = false;
		mLighter.reset();

		mListener->onRenderStatus(buf);
		mListener->onRenderDone();
	}
}
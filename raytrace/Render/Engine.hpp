#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Color.hpp"
#include "Object/Scene.hpp"
#include "Object/Radiance.hpp"

#include "Render/Framebuffer.hpp"
#include "Render/Job.hpp"

#include "Lighter/Master.hpp"

#include <set>
#include <thread>
#include <mutex>
#include <random>
#include <deque>

#include <windows.h>

namespace Render {
	class Engine
	{
	public:
		class Listener {
		public:
			virtual void onRenderDone() = 0;
			virtual void onRenderStatus(const char *message) = 0;
		};

		struct Settings
		{
			int width;
			int height;
			bool lighting;
			int minSamples;
			int maxSamples;
			float sampleThreshold;
			Lighter::Master::Settings lighterSettings;
		};

		Engine(const Object::Scene &scene);

		void stop();

		const Object::Scene &scene() const;

		bool rendering() const;

		void startRender(Listener *listener);
		void setSettings(const Settings &settings);

		const Settings &settings() const;
		Framebuffer &framebuffer();

		Object::Color toneMap(const Object::Radiance &radiance) const;

	private:
		void addJob(std::unique_ptr<Job> job);
		void runThread();

		void renderPixel(int x, int y, Sampler &sampler);
		void renderDone();

		const Object::Scene &mScene;
		Settings mSettings;
		Listener *mListener;
		std::unique_ptr<Framebuffer> mFramebuffer;
		bool mRendering;
		DWORD mStartTime;
		std::unique_ptr<Lighter::Master> mLighter;

		std::mutex mMutex;
		std::condition_variable mConditionVariable;
		std::vector<std::unique_ptr<std::thread>> mThreads;
		std::unique_ptr<Job> mCurrentJob;
		std::deque<std::unique_ptr<Job>> mJobs;
		int mNumRunningThreads;
		bool mStopThreads;
	};
}

#endif
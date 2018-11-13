#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Color.hpp"
#include "Object/Scene.hpp"
#include "Object/Radiance.hpp"

#include "Render/Framebuffer.hpp"
#include "Render/Tracer.hpp"
#include "Lighter/Master.hpp"

#include <set>
#include <thread>
#include <mutex>
#include <random>

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

		class Thread
		{
		public:
			Thread(Engine &engine, std::function<void(Thread &, int, int)> pixelFunction);

			void start(int startX, int startY, int width, int height);

			Tracer &tracer();
			Sampler &sampler();

		private:
			void run();

			int mStartX;
			int mStartY;
			int mWidth;
			int mHeight;
			bool mStarted;
			Engine &mEngine;
			std::thread mThread;
			Sampler mSampler;
			Tracer mTracer;
			std::function<void(Thread &, int, int)> mPixelFunction;
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

		const Object::Scene &scene() const;

		bool rendering() const;

		void startRender(Listener *listener);
		bool threadDone(Thread *thread);
		void setSettings(const Settings &settings);

		const Settings &settings() const;
		Framebuffer &framebuffer();

		Object::Color toneMap(const Object::Radiance &radiance) const;

	private:
		void getBlock(int block, int &x, int &y, int &w, int &h);
		int widthInBlocks();
		int heightInBlocks();

		void beginPhase();
		void endPhase();

		void renderPixel(Thread &thread, int x, int y);
		void prerenderPixel(Thread &thread, int x, int y);

		const Object::Scene &mScene;
		Settings mSettings;
		Listener *mListener;
		std::unique_ptr<Framebuffer> mFramebuffer;
		enum class State {
			Stopped,
			Prerender,
			Render
		};
		State mState;
		DWORD mStartTime;
		std::mutex mMutex;
		std::set<std::unique_ptr<Thread>> mThreads;
		int mBlocksStarted;
		std::unique_ptr<Lighter::Master> mLighter;
	};
}

#endif
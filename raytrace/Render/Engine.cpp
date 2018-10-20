#define NOMINMAX
#include "Render/Engine.hpp"

#include "Lighter/DiffuseDirect.hpp"
#include "Lighter/DiffuseIndirect.hpp"
#include "Lighter/Radiant.hpp"
#include "Lighter/Specular.hpp"

#include <algorithm>
#include <memory>

namespace Render {
	static const int BLOCK_SIZE = 64;

	Engine::Thread::Thread(Engine &engine, std::function<void(Thread &, int, int)> pixelFunction)
		: mEngine(engine)
		, mTracer(engine.scene(), engine.settings().width, engine.settings().height)
		, mPixelFunction(pixelFunction)
	{
		mStarted = false;
	}

	void Engine::Thread::start(int startX, int startY, int width, int height)
	{
		mStartX = startX;
		mStartY = startY;
		mWidth = width;
		mHeight = height;

		if (!mStarted) {
			mStarted = true;
			mThread = std::thread([=] { run(); });
			mThread.detach();
		}
	}

	void Engine::Thread::run()
	{
		while (true) {
			for (int y = mStartY; y < mStartY + mHeight; y++) {
				for (int x = mStartX; x < mStartX + mWidth; x++) {
					mPixelFunction(*this, x, y);
				}
			}

			bool stop = mEngine.threadDone(this);
			if (stop) {
				break;
			}
		}
	}

	Tracer &Engine::Thread::tracer()
	{
		return mTracer;
	}

	std::default_random_engine &Engine::Thread::randomEngine()
	{
		return mRandomEngine;
	}

	Engine::Engine(const Object::Scene &scene)
		: mScene(scene)
	{
		mState = State::Stopped;
		mFramebuffer = std::make_unique<Framebuffer>(0, 0);
	}

	const Object::Scene &Engine::scene() const
	{
		return mScene;
	}

	void Engine::startRender(Listener *listener)
	{
		mListener = listener;
		mState = State::Prerender;
		mListener->onRenderStatus("");

		mStartTime = GetTickCount();

		mLighter = std::make_unique<Lighter::Master>(mSettings.lighterSettings);

		beginPhase();
	}

	bool Engine::rendering() const
	{
		return mState != State::Stopped;
	}

	bool Engine::threadDone(Thread *doneThread)
	{
		bool ret;

		std::lock_guard<std::mutex> guard(mMutex);

		if (mBlocksStarted < widthInBlocks() * heightInBlocks()) {
			int x, y;
			int w, h;

			getBlock(mBlocksStarted, x, y, w, h);
			doneThread->start(x, y, w, h);
			mBlocksStarted++;
			ret = false;
		}
		else {
			for (const std::unique_ptr<Thread> &thread : mThreads) {
				if (thread.get() == doneThread) {
					mThreads.erase(thread);
				}
			}

			if (mThreads.size() == 0) {
				endPhase();
			}
			ret = true;
		}

		return ret;
	}

	void Engine::getBlock(int block, int &x, int &y, int &w, int &h)
	{
		int row = block / widthInBlocks();
		int col = block % widthInBlocks();

		x = col * BLOCK_SIZE;
		y = row * BLOCK_SIZE;
		w = std::min(BLOCK_SIZE, mSettings.width - x);
		h = std::min(BLOCK_SIZE, mSettings.height - y);
	}

	void Engine::beginPhase()
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		int numThreads = sysinfo.dwNumberOfProcessors;
		mBlocksStarted = 0;
		for (int i = 0; i<numThreads; i++) {
			int x, y;
			int w, h;
			getBlock(mBlocksStarted, x, y, w, h);

			std::function<void(Thread&, int, int)> func;
			switch (mState) {
			case State::Prerender:
				func = [&](Thread &thread, int x, int y) { prerenderPixel(thread, x, y); };
				break;

			case State::Render:
				func = [&](Thread &thread, int x, int y) { renderPixel(thread, x, y); };
				break;
			}
			std::unique_ptr<Thread> thread = std::make_unique<Thread>(*this, func);
			thread->start(x, y, w, h);
			mThreads.insert(std::move(thread));
			mBlocksStarted++;
		}
	}

	void Engine::endPhase()
	{
		switch (mState) {
		case State::Prerender:
			mState = State::Render;
			beginPhase();
			break;

		case State::Render:
		{
			mState = State::Stopped;

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

			mListener->onRenderStatus(buf);
			mListener->onRenderDone();
			break;
		}

		default:
			break;
		}
	}

	int Engine::widthInBlocks()
	{
		return (mSettings.width + BLOCK_SIZE - 1) / BLOCK_SIZE;
	}

	int Engine::heightInBlocks()
	{
		return (mSettings.height + BLOCK_SIZE - 1) / BLOCK_SIZE;
	}

	const Engine::Settings &Engine::settings() const
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

	Object::Color Engine::toneMap(const Object::Radiance &radiance) const
	{
		float red = radiance.red() / (radiance.red() + 1);
		float green = radiance.green() / (radiance.green() + 1);
		float blue = radiance.blue() / (radiance.blue() + 1);

		return Object::Color(red, green, blue);
	}

	void Engine::prerenderPixel(Thread &thread, int x, int y)
	{
		Object::Color color;

		Math::Ray ray = thread.tracer().createCameraRay(x, y);
		Object::Intersection intersection = thread.tracer().intersect(ray);
		if (intersection.valid())
		{
			if (mSettings.lighting && mLighter->prerender(intersection, thread.tracer())) {
				color = Object::Color(1, 1, 1);
			}
		}

		mFramebuffer->setPixel(x, y, color);
	}

	void Engine::renderPixel(Thread &thread, int x, int y)
	{
		Object::Color color;
		for (int u = 0; u < mSettings.antialiasSamples; u++) {
			for (int v = 0; v < mSettings.antialiasSamples; v++) {
				Math::Ray ray = thread.tracer().createCameraRay(x + (float)u / mSettings.antialiasSamples, y + (float)v / mSettings.antialiasSamples);
				Object::Intersection intersection = thread.tracer().intersect(ray);

				if (intersection.valid())
				{
					if (mSettings.lighting) {
						Object::Radiance radiance = mLighter->light(intersection, thread.tracer(), 0);
						color += toneMap(radiance);
					}
					else {
						color += intersection.albedo();
					}
				}
			}
		}
		color = color / (mSettings.antialiasSamples * mSettings.antialiasSamples);

		mFramebuffer->setPixel(x, y, color);
	}
}
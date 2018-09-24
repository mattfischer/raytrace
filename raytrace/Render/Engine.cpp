#define NOMINMAX
#include "Render/Engine.hpp"

#include "Render/ThreadRender.hpp"
#include "Render/ThreadPrerender.hpp"

#include "Trace/Lighter/Direct.hpp"
#include "Trace/Lighter/Indirect.hpp"
#include "Trace/Lighter/Radiant.hpp"
#include "Trace/Lighter/Specular.hpp"

#include <algorithm>
#include <memory>

namespace Render {

static const int BLOCK_SIZE = 64;

Engine::Thread::Thread(Engine &engine)
	: mEngine(engine)
{
	mStarted = false;
}

const Engine &Engine::Thread::engine() const
{
	return mEngine;
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
				Object::Color color = renderPixel(x, y);
				mEngine.framebuffer().setPixel(x, y, color);
			}
		}

		bool stop = mEngine.threadDone(this);
		if (stop) {
			break;
		}
	}
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

	mLighters.clear();
	if (mSettings.directLighting) {
		mLighters.push_back(std::make_unique<Trace::Lighter::Direct>(mSettings.directSamples));
	}

	if (mSettings.indirectLighting) {
		mLighters.push_back(std::make_unique<Trace::Lighter::Indirect>(mSettings.indirectSamples, mSettings.indirectDirectSamples, mSettings.irradianceCaching, mSettings.irradianceCacheThreshold));
	}

	if (mSettings.radiantLighting) {
		mLighters.push_back(std::make_unique<Trace::Lighter::Radiant>());
	}

	if (mSettings.specularLighting) {
		mLighters.push_back(std::make_unique<Trace::Lighter::Specular>(*this, mSettings.maxRayGeneration));
	}

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

		std::unique_ptr<Thread> thread;
		switch (mState) {
		case State::Prerender:
			thread = std::make_unique<ThreadPrerender>(*this);
			break;
		case State::Render:
			thread = std::make_unique<ThreadRender>(*this);
			break;
		default:
			break;
		}
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
		sprintf_s(buf, sizeof(buf), "Render time: %.3fs", (endTime - mStartTime) / 1000.0f);

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

const std::vector<std::unique_ptr<Trace::Lighter::Base>> &Engine::lighters() const
{
	return mLighters;
}

Object::Color Engine::toneMap(const Object::Radiance &radiance) const
{
	float red = radiance.red() / (radiance.red() + 1);
	float green = radiance.green() / (radiance.green() + 1);
	float blue = radiance.blue() / (radiance.blue() + 1);

	return Object::Color(red, green, blue);
}

Object::Radiance Engine::traceRay(const Math::Ray &ray, Trace::Tracer &tracer)  const
{
	Object::Intersection intersection = tracer.intersect(ray);

	Object::Radiance radiance;
	if (intersection.valid())
	{
		for (const std::unique_ptr<Trace::Lighter::Base> &lighter : mLighters) {
			radiance += lighter->light(intersection, tracer);
		}
	}

	return radiance;
}


}
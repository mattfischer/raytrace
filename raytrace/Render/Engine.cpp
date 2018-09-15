#define NOMINMAX
#include "Render/Engine.hpp"

#include "Render/ThreadRender.hpp"
#include "Render/ThreadPrerender.hpp"

#include <algorithm>
#include <memory>

namespace Render {

static const int BLOCK_SIZE = 64;

Engine::Engine(const Object::Scene &scene)
	: mScene(scene)
{
	mState = State::Stopped;
	InitializeCriticalSection(&mCritSec);
}

Engine::~Engine()
{
	DeleteCriticalSection(&mCritSec);
}

void Engine::startRender(Framebuffer *framebuffer, Listener *listener)
{
	mListener = listener;
	mFramebuffer = framebuffer;
	mState = State::Prerender;
	mListener->onRenderStatus("");

	mStartTime = GetTickCount();
	mRenderData.irradianceCache.clear();
	mRenderData.irradianceCache.setThreshold(mSettings.irradianceCacheThreshold);

	beginPhase();
}

bool Engine::rendering() const
{
	return mState != State::Stopped;
}

bool Engine::threadDone(Thread *doneThread)
{
	bool ret;

	EnterCriticalSection(&mCritSec);

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
	LeaveCriticalSection(&mCritSec);

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
			thread = std::make_unique<ThreadPrerender>(this, mScene, mSettings, mRenderData, mFramebuffer);
			break;
		case State::Render:
			thread = std::make_unique<ThreadRender>(this, mScene, mSettings, mRenderData, mFramebuffer);
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

Trace::Tracer Engine::createTracer()
{
	return Trace::Tracer(mScene, mSettings, mRenderData);
}

Trace::Tracer::Settings &Engine::settings()
{
	return mSettings;
}

void Engine::setSettings(const Trace::Tracer::Settings &settings)
{
	mSettings = settings;
}

}
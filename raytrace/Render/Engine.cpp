#define NOMINMAX
#include "Render/Engine.hpp"

#include "Object/Color.hpp"

#include <algorithm>

namespace Render {

static const int BLOCK_SIZE = 64;

Engine::Engine(const Object::Scene &scene)
	: mScene(scene)
{
	mRendering = false;
	InitializeCriticalSection(&mCritSec);
}

Engine::~Engine()
{
	DeleteCriticalSection(&mCritSec);
}

void Engine::startPrerender(unsigned char *bits, Listener *listener)
{
	mStartTime = GetTickCount();
	mRenderData.irradianceCache.clear();
	mRenderData.irradianceCache.setThreshold(mSettings.irradianceCacheThreshold);

	mListener = listener;

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numThreads = sysinfo.dwNumberOfProcessors;
	mBlocksStarted = 0;
	for (int i = 0; i<numThreads; i++) {
		int x, y;
		int w, h;
		getBlock(mBlocksStarted, x, y, w, h);
		std::unique_ptr<PrerenderThread> thread = std::make_unique<PrerenderThread>(*this, mScene, mSettings, mRenderData, bits);
		thread->start(x, y, w, h);
		mPrerenderThreads.insert(std::move(thread));
		mBlocksStarted++;
	}
}

void Engine::startRender(unsigned char *bits, Listener *listener)
{
	mBits = bits;
	mListener = listener;
	mRendering = true;
	mListener->onRenderStatus("");

	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	int numThreads = sysinfo.dwNumberOfProcessors;
	mBlocksStarted = 0;
	for(int i=0; i<numThreads; i++) {
		int x, y;
		int w, h;
		getBlock(mBlocksStarted, x, y, w, h);
		std::unique_ptr<Thread> thread = std::make_unique<Thread>(*this, mScene, mSettings, mRenderData, mBits);
		thread->start(x, y, w, h);
		mThreads.insert(std::move(thread));
		mBlocksStarted++;
	}
}

bool Engine::rendering() const
{
	return mRendering;
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
			mRendering = false;

			DWORD endTime = GetTickCount();
			char buf[256];
			sprintf_s(buf, sizeof(buf), "Render time: %.3fs", (endTime - mStartTime) / 1000.0f);

			mListener->onRenderStatus(buf);
			mListener->onRenderDone();
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

int Engine::widthInBlocks()
{
	return (mSettings.width + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

int Engine::heightInBlocks()
{
	return (mSettings.height + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

bool Engine::prerenderThreadDone(PrerenderThread *doneThread)
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
		for (const std::unique_ptr<PrerenderThread> &thread : mPrerenderThreads) {
			if (thread.get() == doneThread) {
				mPrerenderThreads.erase(thread);
			}
		}

		if (mPrerenderThreads.size() == 0) {
			mListener->onPrerenderDone();
		}
		ret = true;
	}
	LeaveCriticalSection(&mCritSec);

	return ret;
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
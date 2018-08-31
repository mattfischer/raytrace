#include "Render/Engine.hpp"

#include "Object/Color.hpp"

namespace Render {

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
	mRenderData.irradianceCache.setThreshold(mSettings.indirectCacheThreshold);

	mListener = listener;
	mPrerenderThread = std::make_unique<PrerenderThread>(*this, mScene, mSettings, mRenderData, bits);
	mPrerenderThread->start();
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
	mNumActiveThreads = numThreads;

	int size = mSettings.height / numThreads;
	for(int i=0; i<numThreads; i++) {
		int startLine = i * size;
		int numLines = (i == numThreads - 1) ? mSettings.height - startLine : size;

		std::unique_ptr<Thread> thread = std::make_unique<Thread>(*this, mScene, mSettings, mRenderData, mBits);
		thread->start(startLine, numLines);
		mThreads.insert(std::move(thread));
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

	Thread *splitThread = 0;

	for(const std::unique_ptr<Thread> &thread : mThreads) {
		if(!splitThread || thread->linesToGo() > splitThread->linesToGo()) {
			splitThread = thread.get();
		}
	}

	if(splitThread->linesToGo() > 10) {
		int transfer = splitThread->linesToGo() / 2;
		int newStart = splitThread->startLine() + splitThread->numLines() - transfer;
		splitThread->setNumLines(splitThread->numLines() - transfer);

		doneThread->start(newStart, transfer);
		ret = false;
	} else {
		for (const std::unique_ptr<Thread> &thread : mThreads) {
			if (thread.get() == doneThread) {
				mThreads.erase(thread);
			}
		}

		if(mThreads.size() == 0) {
			mRendering = false;

			DWORD endTime = GetTickCount();
			char buf[256];
			sprintf_s(buf, sizeof(buf), "Render time: %ims", endTime - mStartTime);

			mListener->onRenderStatus(buf);
			mListener->onRenderDone();
		}
		ret = true;
	}

	LeaveCriticalSection(&mCritSec);

	return ret;
}

void Engine::prerenderThreadDone()
{
	mListener->onPrerenderDone();
	mPrerenderThread.reset();
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
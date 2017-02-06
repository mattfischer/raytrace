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

void Engine::startRender(const Trace::Tracer::Settings &settings, unsigned char *bits, Listener *listener)
{
	mSettings = settings;
	mBits = bits;
	mListener = listener;
	mRendering = true;
	mStartTime = GetTickCount();
	mListener->onRenderStatus("");

	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	int numThreads = sysinfo.dwNumberOfProcessors;
	mNumActiveThreads = numThreads;

	int size = mSettings.height / numThreads;
	for(int i=0; i<numThreads; i++) {
		int startLine = i * size;
		int numLines = (i == numThreads - 1) ? mSettings.height - startLine : size;

		std::unique_ptr<Thread> thread = std::make_unique<Thread>(*this, mScene, mSettings, mBits);
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

}
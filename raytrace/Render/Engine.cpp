#include "Render/Engine.hpp"

#include "Object/Color.hpp"

namespace Render {

Engine::Engine()
{
	mRendering = false;
	InitializeCriticalSection(&mCritSec);
}

Engine::~Engine()
{
	DeleteCriticalSection(&mCritSec);
}

void Engine::startRender(Object::Scene *scene, const Trace::Tracer::Settings &settings, unsigned char *bits, Listener *listener)
{
	mScene = scene;
	mSettings = settings;
	mBits = bits;
	mListener = listener;
	mRendering = true;
	mStartTime = GetTickCount();
	mListener->onRenderStatus("");

	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	mNumThreads = sysinfo.dwNumberOfProcessors;
	mNumActiveThreads = mNumThreads;
	mThreads = new Thread*[mNumThreads];

	int size = mSettings.height / mNumThreads;
	for(int i=0; i<mNumThreads; i++) {
		int startLine = i * size;
		int numLines = (i == mNumThreads - 1) ? mSettings.height - startLine : size;

		mThreads[i] = new Thread(this, mScene, mSettings, mBits);
		mThreads[i]->start(startLine, numLines);
	}
}

bool Engine::rendering() const
{
	return mRendering;
}

bool Engine::threadDone(Thread *thread)
{
	bool ret;

	EnterCriticalSection(&mCritSec);

	Thread *splitThread = 0;
	int maxToGo = 0;
	int thisThread = -1;
	for(int i=0; i<mNumThreads; i++) {
		if(!mThreads[i]) {
			continue;
		}

		if(mThreads[i] == thread) {
			thisThread = i;
		}

		int toGo = mThreads[i]->numLines() - (mThreads[i]->currentLine() - mThreads[i]->startLine());
		if(toGo > maxToGo) {
			splitThread = mThreads[i];
			maxToGo = toGo;
		}
	}

	if(maxToGo > 10) {
		int transfer = maxToGo / 2;
		int newStart = splitThread->startLine() + splitThread->numLines() - transfer;
		splitThread->setNumLines(splitThread->numLines() - transfer);

		thread->start(newStart, transfer);
		ret = false;
	} else {
		mThreads[thisThread] = 0;
		delete thread;

		mNumActiveThreads--;
		if(mNumActiveThreads == 0) {
			mRendering = false;
			delete[] mThreads;

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
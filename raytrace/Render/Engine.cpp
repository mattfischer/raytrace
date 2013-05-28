#include "Render/Engine.hpp"

#include "Object/Color.hpp"

namespace Render {

Engine::Engine()
{
	mRendering = false;
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

	int size = mSettings.height / mNumThreads;
	for(int i=0; i<mNumThreads; i++) {
		int startLine = i * size;
		int numLines = (i == mNumThreads - 1) ? mSettings.height - startLine : size;

		Thread *thread = new Thread(this, mScene, mSettings, mBits);
		thread->start(startLine, numLines);
	}
}

bool Engine::rendering() const
{
	return mRendering;
}

void Engine::threadDone(Thread *thread)
{
	delete thread;

	LONG numThreads = InterlockedDecrement(&mNumThreads);
	if(numThreads == 0) {
		mRendering = false;

		DWORD endTime = GetTickCount();
		char buf[256];
		sprintf_s(buf, sizeof(buf), "Render time: %ims", endTime - mStartTime);

		mListener->onRenderStatus(buf);
		mListener->onRenderDone();
	}
}

}
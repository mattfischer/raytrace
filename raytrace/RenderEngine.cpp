#include "RenderEngine.hpp"

#include "Object/Color.hpp"

#include <process.h>
#include <windows.h>

class WorkerThread {
public:
	WorkerThread(RenderEngine *engine);
	void start();

private:
	static void kickstart(void *data);
	void run();

	RenderEngine *mEngine;
};

WorkerThread::WorkerThread(RenderEngine *engine)
{
	mEngine = engine;
}

void WorkerThread::start()
{
	_beginthread(kickstart, 0, this);
}

void WorkerThread::kickstart(void *data)
{
	WorkerThread *obj = (WorkerThread*)data;
	obj->run();
}

void WorkerThread::run()
{
	Trace::Tracer tracer(mEngine->scene(), mEngine->settings());
	int width = mEngine->settings().width;
	int height = mEngine->settings().height;
	while(true) {
		LONG pixel = InterlockedIncrement(mEngine->nextPixel()) - 1;
		int x = pixel % width;
		int y = pixel / width;

		if(y >= height) {
			break;
		}

		Object::Color c = tracer.tracePixel(x, y);

		int scany = height - y - 1;
		mEngine->bits()[(scany * width + x) * 3 + 0] = c.blue() * 0xFF;
		mEngine->bits()[(scany * width + x) * 3 + 1] = c.green() * 0xFF;
		mEngine->bits()[(scany * width + x) * 3 + 2] = c.red() * 0xFF;
	}

	mEngine->threadDone(this);
}

RenderEngine::RenderEngine()
{
	mRendering = false;
}

void RenderEngine::startRender(Object::Scene *scene, const Trace::Tracer::Settings &settings, unsigned char *bits, Listener *listener)
{
	mScene = scene;
	mSettings = settings;
	mBits = bits;
	mListener = listener;
	mRendering = true;
	mNextPixel = 0;
	mStartTime = GetTickCount();
	mListener->onRenderStatus("");

	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	mNumThreads = sysinfo.dwNumberOfProcessors;

	for(int i=0; i<mNumThreads; i++) {
		WorkerThread *thread = new WorkerThread(this);
		thread->start();
	}
}

Object::Scene *RenderEngine::scene() const
{
	return mScene;
}

const Trace::Tracer::Settings &RenderEngine::settings() const
{
	return mSettings;
}

unsigned char *RenderEngine::bits() const
{
	return mBits;
}

bool RenderEngine::rendering() const
{
	return mRendering;
}

LONG *RenderEngine::nextPixel()
{
	return &mNextPixel;
}

void RenderEngine::threadDone(WorkerThread *thread)
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
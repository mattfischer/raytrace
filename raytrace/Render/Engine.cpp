#include "Render/Engine.hpp"

#include "Object/Color.hpp"

#include <process.h>
#include <windows.h>

namespace Render {

class WorkerThread {
public:
	WorkerThread(Engine *engine)
	{
		mEngine = engine;
	}

	void start()
	{
		_beginthread(kickstart, 0, this);
	}

private:
	static void kickstart(void *data)
	{
		WorkerThread *obj = (WorkerThread*)data;
		obj->run();
	}

	void run()
	{
		mEngine->renderThread();
		mEngine->threadDone(this);
	}

	Engine *mEngine;
};

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

bool Engine::rendering() const
{
	return mRendering;
}

void Engine::threadDone(WorkerThread *thread)
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

void Engine::renderThread()
{
	Trace::Tracer tracer(mScene, mSettings);
	int width = mSettings.width;
	int height = mSettings.height;
	while(true) {
		LONG pixel = InterlockedIncrement(&mNextPixel) - 1;
		int x = pixel % width;
		int y = pixel / width;

		if(y >= height) {
			break;
		}

		Object::Color corners[4];
		corners[0] = tracer.tracePixel(x, y);
		corners[1] = tracer.tracePixel(x + 1, y);
		corners[2] = tracer.tracePixel(x, y + 1);
		corners[3] = tracer.tracePixel(x + 1, y + 1);

		Object::Color c = antialiasPixel(tracer, x + 0.5f, y + 0.5f, 1.0f, corners);
		int height = mSettings.height;
		int width = mSettings.width;
		int scany = height - y - 1;
		mBits[(scany * width + x) * 3 + 0] = c.blue() * 0xFF;
		mBits[(scany * width + x) * 3 + 1] = c.green() * 0xFF;
		mBits[(scany * width + x) * 3 + 2] = c.red() * 0xFF;
	}
}

Object::Color Engine::antialiasPixel(const Trace::Tracer &tracer, float x, float y, float size, const Object::Color corners[4], int generation) const
{
	Object::Color ret;

	if(shouldAntialias(corners, size) && generation < mSettings.maxAAGen) {
		Object::Color subs[9];
		int idx = 0;
		for(int j=-1; j<=1; j++) {
			for(int i=-1; i<=1; i++) {
				switch(idx) {
					case 0: subs[idx] = corners[0]; break;
					case 2: subs[idx] = corners[1]; break;
					case 6: subs[idx] = corners[2]; break;
					case 8: subs[idx] = corners[3]; break;
					default: subs[idx] = tracer.tracePixel(x + i * size / 2, y + j * size / 2); break;
				}
				idx++;
			}
		}

		Object::Color subpixels[4];
		Object::Color subcorners[4];

		subcorners[0] = subs[0]; subcorners[1] = subs[1]; subcorners[2] = subs[3]; subcorners[3] = subs[4];
		subpixels[0] = antialiasPixel(tracer, x - size/4, y - size/4, size / 2, subcorners, generation + 1);

		subcorners[0] = subs[1]; subcorners[1] = subs[2]; subcorners[2] = subs[4]; subcorners[3] = subs[5];
		subpixels[1] = antialiasPixel(tracer, x + size/4, y - size/4, size / 2, subcorners, generation + 1);

		subcorners[0] = subs[3]; subcorners[1] = subs[4]; subcorners[2] = subs[6]; subcorners[3] = subs[7];
		subpixels[2] = antialiasPixel(tracer, x - size/4, y + size/4, size / 2, subcorners, generation + 1);

		subcorners[0] = subs[4]; subcorners[1] = subs[5]; subcorners[2] = subs[7]; subcorners[3] = subs[8];
		subpixels[3] = antialiasPixel(tracer, x + size/4, y + size/4, size / 2, subcorners, generation + 1);

		for(int i=0; i<4; i++) {
			ret = ret + subpixels[i];
		}
		ret = ret / 4;
	} else {
		for(int i=0; i<4; i++) {
			ret = ret + corners[i];
		}
		ret = ret / 4;
	}

	return ret;
}

static float colorMag(const Object::Color &x, const Object::Color &y)
{
	float r = x.red() - y.red();
	float g = x.green() - y.green();
	float b = x.blue() - y.blue();

	return r * r + g * g + b * b;
}

bool Engine::shouldAntialias(const Object::Color corners[4], float size) const
{
	Object::Color center;
	float dist = 0;
	for(int i=0; i<4; i++) {
		center = center + corners[i];
	}
	center = center / 4;

	for(int i=0; i<4; i++) {
		dist += colorMag(center, corners[i]);
	}

	return dist > size * mSettings.threshold * mSettings.threshold;
}

}
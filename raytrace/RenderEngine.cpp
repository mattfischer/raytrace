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

		Object::Color corners[4];
		corners[0] = tracer.tracePixel(x, y);
		corners[1] = tracer.tracePixel(x + 1, y);
		corners[2] = tracer.tracePixel(x, y + 1);
		corners[3] = tracer.tracePixel(x + 1, y + 1);

		Object::Color c = mEngine->antialiasPixel(tracer, x + 0.5f, y + 0.5f, 1.0f, corners);
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

Object::Color RenderEngine::antialiasPixel(const Trace::Tracer &tracer, float x, float y, float size, const Object::Color corners[4], int generation) const
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

bool RenderEngine::shouldAntialias(const Object::Color corners[4], float size) const
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

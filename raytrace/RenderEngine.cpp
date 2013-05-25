#include "RenderEngine.hpp"

#include "Object/Color.hpp"

#include <process.h>

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
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			Object::Color c = tracer.tracePixel(x, y);

			int scany = height - y - 1;
			mEngine->bits()[(scany * width + x) * 3 + 0] = c.blue() * 0xFF;
			mEngine->bits()[(scany * width + x) * 3 + 1] = c.green() * 0xFF;
			mEngine->bits()[(scany * width + x) * 3 + 2] = c.red() * 0xFF;
		}
	}

	mEngine->threadDone(this);
}

RenderEngine::RenderEngine()
{
}

void RenderEngine::startRender(Object::Scene *scene, const Trace::Tracer::Settings &settings, unsigned char *bits, Listener *listener)
{
	mScene = scene;
	mSettings = settings;
	mBits = bits;
	mListener = listener;

	WorkerThread *thread = new WorkerThread(this);
	thread->start();
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

void RenderEngine::threadDone(WorkerThread *thread)
{
	delete thread;

	mListener->onRenderDone();
}
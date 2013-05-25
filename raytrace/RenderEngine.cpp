#include "RenderEngine.hpp"

#include "Object/Color.hpp"

#include <process.h>

RenderEngine::RenderEngine(Object::Scene *scene, const Trace::Tracer::Settings &settings)
{
	mScene = scene;
	mSettings = settings;
}

struct ThreadData {
	RenderEngine *engine;
	unsigned char *bits;
	RenderEngine::DoneCallback doneCallback;
	void *data;
};

static void threadFunc(void *data)
{
	ThreadData *td = (ThreadData*)data;

	Trace::Tracer tracer(td->engine->scene(), td->engine->settings());
	int width = td->engine->settings().width;
	int height = td->engine->settings().height;
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			Object::Color c = tracer.tracePixel(x, y);

			int scany = height - y - 1;
			td->bits[(scany * width + x) * 3 + 0] = c.blue() * 0xFF;
			td->bits[(scany * width + x) * 3 + 1] = c.green() * 0xFF;
			td->bits[(scany * width + x) * 3 + 2] = c.red() * 0xFF;
		}
	}

	td->doneCallback(td->engine, td->data);

	delete td;
}

void RenderEngine::render(unsigned char *bits, DoneCallback doneCallback, void *data)
{
	ThreadData *td = new ThreadData;

	td->bits = bits;
	td->doneCallback = doneCallback;
	td->data = data;
	td->engine = this;

	_beginthread(threadFunc, 0, td);
}

Object::Scene *RenderEngine::scene() const
{
	return mScene;
}

const Trace::Tracer::Settings &RenderEngine::settings() const
{
	return mSettings;
}
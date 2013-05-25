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
	RenderEngine::LineCallback lineCallback;
	RenderEngine::DoneCallback doneCallback;
	void *data;
};

static void threadFunc(void *data)
{
	ThreadData *td = (ThreadData*)data;

	Trace::Tracer tracer(td->engine->scene(), td->engine->settings());

	for(int y=0; y<td->engine->settings().height; y++)
	{
		for(int x=0; x<td->engine->settings().width; x++)
		{
			Object::Color c = tracer.tracePixel(x, y);

			td->bits[x*3] = c.blue() * 0xFF;
			td->bits[x*3 + 1] = c.green() * 0xFF;
			td->bits[x*3 + 2] = c.red() * 0xFF;
		}

		td->lineCallback(y, td->data);
	}

	td->doneCallback(td->engine, td->data);

	delete td;
}

void RenderEngine::render(unsigned char *bits, LineCallback lineCallback, DoneCallback doneCallback, void *data)
{
	ThreadData *td = new ThreadData;

	td->bits = bits;
	td->lineCallback = lineCallback;
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
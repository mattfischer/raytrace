#include "Render/Thread.hpp"
#include "Render/Engine.hpp"

#include "Trace/Lighter/Utils.hpp"

#include <process.h>

namespace Render {

Thread::Thread(Engine &engine, const Object::Scene &scene, const Trace::Tracer::Settings &settings, Trace::Tracer::RenderData &renderData, Framebuffer *framebuffer)
: mEngine(engine),
  mSettings(settings),
  mRenderData(renderData),
  mTracer(scene, settings, renderData)
{
	mFramebuffer = framebuffer;
	mStarted = false;
}

void Thread::start(int startX, int startY, int width, int height)
{
	mStartX = startX;
	mStartY = startY;
	mWidth = width;
	mHeight = height;

	if(!mStarted) {
		mStarted = true;
		_beginthread(kickstart, 0, this);
	}
}

void Thread::kickstart(void *data)
{
	Thread *obj = (Thread*)data;
	obj->run();
}

void Thread::run()
{
	while(true) {
		doRender();

		bool stop = mEngine.threadDone(this);
		if(stop) {
			break;
		}
	}
}

void Thread::doRender()
{
	int width = mSettings.width;
	int height = mSettings.height;

	std::default_random_engine randomEngine;

	for (int y = mStartY; y < mStartY + mHeight; y++) {
		for (int x = mStartX; x < mStartX + mWidth; x++) {
			Object::Color color;
			for (int i = 0; i < mSettings.antialiasSamples; i++) {
				float u;
				float v;

				Trace::Lighter::Utils::stratifiedSamples(i, mSettings.antialiasSamples, u, v, randomEngine);
				color += mTracer.tracePixel(x + u, y + v);
			}
			color = color / mSettings.antialiasSamples;

			mFramebuffer->setPixel(x, y, color);
		}
	}
}
}
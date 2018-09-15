#include "Render/Thread.hpp"
#include "Render/Engine.hpp"

#include <process.h>

namespace Render {

	PrerenderThread::PrerenderThread(Engine &engine, const Object::Scene &scene, const Trace::Tracer::Settings &settings, Trace::Tracer::RenderData &renderData, Framebuffer *framebuffer)
		: mEngine(engine),
		mSettings(settings),
		mRenderData(renderData),
		mTracer(scene, settings, renderData)
	{
		mFramebuffer = framebuffer;
		mStarted = false;
	}

	void PrerenderThread::start(int startX, int startY, int width, int height)
	{
		mStartX = startX;
		mStartY = startY;
		mWidth = width;
		mHeight = height;

		if (!mStarted) {
			mStarted = true;
			_beginthread(kickstart, 0, this);
		}
	}

	void PrerenderThread::kickstart(void *data)
	{
		PrerenderThread *obj = (PrerenderThread*)data;
		obj->run();
	}

	void PrerenderThread::run()
	{
		while (true) {
			doPrerender();

			bool stop = mEngine.prerenderThreadDone(this);
			if (stop) {
				break;
			}
		}
	}

	void PrerenderThread::doPrerender()
	{
		for (int y = mStartY; y < mStartY + mHeight; y++) {
			for (int x = mStartX; x < mStartX + mWidth; x++) {
				if (mTracer.prerenderPixel(x, y)) {
					mFramebuffer->setPixel(x, y, Object::Color(1, 1, 1));
				}
			}
		}
	}
}
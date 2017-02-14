#include "Render/Thread.hpp"
#include "Render/Engine.hpp"

#include <process.h>

namespace Render {

	PrerenderThread::PrerenderThread(Engine &engine, const Object::Scene &scene, const Trace::Tracer::Settings &settings, Trace::Tracer::RenderData &renderData, unsigned char *bits)
		: mEngine(engine),
		mSettings(settings),
		mRenderData(renderData),
		mTracer(scene, settings, renderData)
	{
		mBits = bits;
		mStarted = false;
	}

	void PrerenderThread::start()
	{
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
		for (int y = 0; y < mSettings.height; y++) {
			for (int x = 0; x < mSettings.width; x++) {
				if (mTracer.prerenderPixel(x, y)) {
					mBits[((mSettings.height - y - 1) * mSettings.width + x) * 3 + 0] = 0xFF;
					mBits[((mSettings.height - y - 1) * mSettings.width + x) * 3 + 1] = 0xFF;
					mBits[((mSettings.height - y - 1) * mSettings.width + x) * 3 + 2] = 0xFF;
				}
			}
		}

		mEngine.prerenderThreadDone();
	}
}
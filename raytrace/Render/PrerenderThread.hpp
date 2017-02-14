#ifndef RENDER_PRERENDER_THREAD_HPP
#define RENDER_PRERENDER_THREAD_HPP

#include "Object/Forwards.hpp"
#include "Render/Forwards.hpp"

#include "Trace/Tracer.hpp"

namespace Render {

	class Engine;
	class PrerenderThread
	{
	public:
		PrerenderThread(Engine &engine, const Object::Scene &scene, const Trace::Tracer::Settings &settings, Trace::Tracer::RenderData &renderData, unsigned char *bits);

		void start();

	private:
		static void kickstart(void *data);

		void run();

		Engine &mEngine;
		const Trace::Tracer::Settings &mSettings;
		Trace::Tracer::RenderData &mRenderData;
		unsigned char *mBits;
		Trace::Tracer mTracer;
		bool mStarted;
	};

}

#endif
#include "ThreadRender.hpp"

#include "Trace/Lighter/Utils.hpp"

namespace Render {

ThreadRender::ThreadRender(Engine &engine)
	: Thread(engine)
{
	mTracer = engine.createTracer();
}

Object::Color ThreadRender::renderPixel(int x, int y)
{
	Object::Color color;
	for (int i = 0; i < mTracer->settings().antialiasSamples; i++) {
		float u;
		float v;

		Trace::Lighter::Utils::stratifiedSamples(i, mTracer->settings().antialiasSamples, u, v, mRandomEngine);
		color += mTracer->tracePixel(x + u, y + v);
	}
	color = color / mTracer->settings().antialiasSamples;
	return color;
}

}
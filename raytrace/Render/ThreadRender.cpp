#include "ThreadRender.hpp"

#include "Trace/Lighter/Utils.hpp"
#include "Trace/Intersection.hpp"
#include "Object/Primitive/Base.hpp"

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
		Trace::Ray ray = mTracer->createCameraRay(x + u, y + v);

		if (mTracer->settings().lighting)
		{
			Object::Radiance radiance = engine().traceRay(ray, *mTracer);
			color += engine().toneMap(radiance);
		}
		else
		{
			Trace::Intersection intersection = mTracer->intersect(ray);
			if (intersection.valid())
			{
				color += intersection.primitive()->surface().albedo().color(intersection.objectPoint());
			}
		}
	}
	color = color / mTracer->settings().antialiasSamples;
	return color;
}

}
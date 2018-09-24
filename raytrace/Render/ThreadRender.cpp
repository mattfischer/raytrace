#include "ThreadRender.hpp"

#include "Trace/Lighter/Utils.hpp"
#include "Trace/Intersection.hpp"
#include "Object/Primitive/Base.hpp"

namespace Render {

ThreadRender::ThreadRender(Engine &engine)
	: Thread(engine)
	, mTracer(engine.scene(), engine.settings().width, engine.settings().height)
{
}

Object::Color ThreadRender::renderPixel(int x, int y)
{
	Object::Color color;
	for (int i = 0; i < engine().settings().antialiasSamples; i++) {
		float u;
		float v;

		Trace::Lighter::Utils::stratifiedSamples(i, engine().settings().antialiasSamples, u, v, mRandomEngine);
		Trace::Ray ray = mTracer.createCameraRay(x + u, y + v);

		if (mLighting)
		{
			Object::Radiance radiance = engine().traceRay(ray, mTracer);
			color += engine().toneMap(radiance);
		}
		else
		{
			Trace::Intersection intersection = mTracer.intersect(ray);
			if (intersection.valid())
			{
				color += intersection.primitive()->surface().albedo().color(intersection.objectPoint());
			}
		}
	}
	color = color / engine().settings().antialiasSamples;
	return color;
}

}
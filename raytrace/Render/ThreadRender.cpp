#include "ThreadRender.hpp"

#include "Lighter/Utils.hpp"
#include "Object/Intersection.hpp"
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

		Lighter::Utils::stratifiedSamples(i, engine().settings().antialiasSamples, u, v, mRandomEngine);
		Math::Ray ray = mTracer.createCameraRay(x + u, y + v);

		if (engine().settings().lighting)
		{
			Object::Radiance radiance = engine().traceRay(ray, mTracer);
			color += engine().toneMap(radiance);
		}
		else
		{
			Object::Intersection intersection = mTracer.intersect(ray);
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
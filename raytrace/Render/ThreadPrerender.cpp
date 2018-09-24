#include "ThreadPrerender.hpp"

namespace Render {

ThreadPrerender::ThreadPrerender(Engine &engine)
	: Thread(engine)
{
	mTracer = engine.createTracer();
}

Object::Color ThreadPrerender::renderPixel(int x, int y)
{
	Trace::Ray ray = mTracer->createCameraRay(x, y);

	Trace::Intersection intersection = mTracer->intersect(ray);

	Object::Color color;
	if (intersection.valid())
	{
		for (const std::unique_ptr<Trace::Lighter::Base> &lighter : engine().lighters()) {
			if (lighter->prerender(intersection, *mTracer)) {
				color = Object::Color(1, 1, 1);
			}
		}
	}

	return color;
}

}
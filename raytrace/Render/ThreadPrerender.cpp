#include "ThreadPrerender.hpp"

namespace Render {

ThreadPrerender::ThreadPrerender(Engine &engine)
	: Thread(engine)
	, mTracer(engine.scene(), engine.settings().width, engine.settings().height)
{
}

Object::Color ThreadPrerender::renderPixel(int x, int y)
{
	Math::Ray ray = mTracer.createCameraRay(x, y);

	Object::Intersection intersection = mTracer.intersect(ray);

	Object::Color color;
	if (intersection.valid())
	{
		for (const std::unique_ptr<Lighter::Base> &lighter : engine().lighters()) {
			if (lighter->prerender(intersection, mTracer)) {
				color = Object::Color(1, 1, 1);
			}
		}
	}

	return color;
}

}
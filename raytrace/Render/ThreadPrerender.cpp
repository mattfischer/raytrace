#include "ThreadPrerender.hpp"

namespace Render {

ThreadPrerender::ThreadPrerender(Engine &engine)
	: Thread(engine)
{
	mTracer = engine.createTracer();
}

Object::Color ThreadPrerender::renderPixel(int x, int y)
{
	Object::Color color;
	if (mTracer->prerenderPixel(x, y)) {
		color = Object::Color(1, 1, 1);
	}
	return color;
}

}
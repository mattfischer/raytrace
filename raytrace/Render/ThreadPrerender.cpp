#include "ThreadPrerender.hpp"

namespace Render {

ThreadPrerender::ThreadPrerender(Thread::Listener *listener, const Object::Scene &scene, const Trace::Tracer::Settings &settings, Trace::Tracer::RenderData &renderData, Framebuffer *framebuffer)
	: Thread(listener, framebuffer), mTracer(scene, settings, renderData)
{}

Object::Color ThreadPrerender::renderPixel(int x, int y)
{
	Object::Color color;
	if (mTracer.prerenderPixel(x, y)) {
		color = Object::Color(1, 1, 1);
	}
	return color;
}

}
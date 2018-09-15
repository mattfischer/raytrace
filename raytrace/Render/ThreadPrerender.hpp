#ifndef RENDER_THREADPRERENDER_HPP
#define RENDER_THREADPRERENDER_HPP

#include "Render/Thread.hpp"

#include "Trace/Tracer.hpp"

namespace Render
{

class ThreadPrerender : public Render::Thread
{
public:
	ThreadPrerender(Thread::Listener *listener, const Object::Scene &scene, const Trace::Tracer::Settings &settings, Trace::Tracer::RenderData &renderData, Framebuffer *framebuffer);

protected:
	virtual Object::Color renderPixel(int x, int y);

private:
	Trace::Tracer mTracer;
};

}

#endif
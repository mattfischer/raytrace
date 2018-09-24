#ifndef RENDER_THREADPRERENDER_HPP
#define RENDER_THREADPRERENDER_HPP

#include "Render/Engine.hpp"

#include "Trace/Tracer.hpp"

namespace Render
{

class ThreadPrerender : public Engine::Thread
{
public:
	ThreadPrerender(Engine &engine);

protected:
	virtual Object::Color renderPixel(int x, int y);

private:
	Trace::Tracer mTracer;
};

}

#endif
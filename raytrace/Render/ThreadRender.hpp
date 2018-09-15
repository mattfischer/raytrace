#ifndef RENDER_THREADRENDER_HPP
#define RENDER_THREADRENDER_HPP

#include "Render/Engine.hpp"
#include "Trace/Tracer.hpp"

#include <random>

namespace Render
{

class ThreadRender : public Engine::Thread
{
public:
	ThreadRender(Engine &engine);

protected:
	virtual Object::Color renderPixel(int x, int y);

private:
	std::unique_ptr<Trace::Tracer> mTracer;
	std::default_random_engine mRandomEngine;
};

}

#endif
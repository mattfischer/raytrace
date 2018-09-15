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
	ThreadRender(Engine *engine, const Object::Scene &scene, const Trace::Tracer::Settings &settings, Trace::Tracer::RenderData &renderData, Framebuffer *framebuffer);

protected:
	virtual Object::Color renderPixel(int x, int y);

private:
	Trace::Tracer mTracer;
	std::default_random_engine mRandomEngine;
};

}

#endif
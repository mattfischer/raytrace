#ifndef RENDER_THREADRENDER_HPP
#define RENDER_THREADRENDER_HPP

#include "Render/Engine.hpp"
#include "Render/Tracer.hpp"

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
	Render::Tracer mTracer;
	std::default_random_engine mRandomEngine;
	bool mLighting;
	int mAntialiasSamples;
};

}

#endif
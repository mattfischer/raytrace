#ifndef RENDER_THREAD_HPP
#define RENDER_THREAD_HPP

#include "Object/Forwards.hpp"
#include "Render/Forwards.hpp"

#include "Trace/Tracer.hpp"

namespace Render {

class Engine;
class Thread
{
public:
	Thread(Engine &engine, const Object::Scene &scene, const Trace::Tracer::Settings &settings, Trace::Tracer::RenderData &renderData, Framebuffer *framebuffer);

	void start(int startX, int startY, int width, int height);

private:
	static void kickstart(void *data);

	void run();
	void doRender();

	Engine &mEngine;
	const Trace::Tracer::Settings &mSettings;
	Trace::Tracer::RenderData &mRenderData;
	Framebuffer *mFramebuffer;
	int mStartX;
	int mStartY;
	int mWidth;
	int mHeight;
	Trace::Tracer mTracer;
	bool mStarted;
};

}

#endif
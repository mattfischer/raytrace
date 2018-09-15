#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Forwards.hpp"
#include "Trace/Forwards.hpp"

#include "Render/Thread.hpp"
#include "Render/PrerenderThread.hpp"
#include "Render/Framebuffer.hpp"

#include <set>

#include <windows.h>

namespace Render {

class Engine
{
public:
	class Listener {
	public:
		virtual void onRenderDone() = 0;
		virtual void onRenderStatus(const char *message) = 0;
		virtual void onPrerenderDone() = 0;
	};

	Engine(const Object::Scene &scene);
	~Engine();

	bool rendering() const;

	void startPrerender(Framebuffer *framebuffer, Listener *listener);
	void startRender(Framebuffer *framebuffer, Listener *listener);
	bool threadDone(Thread *thread);
	bool prerenderThreadDone(PrerenderThread *thread);
	void setSettings(const Trace::Tracer::Settings &settings);

	Trace::Tracer createTracer();
	Trace::Tracer::Settings &settings();

private:
	void getBlock(int block, int &x, int &y, int &w, int &h);
	int widthInBlocks();
	int heightInBlocks();

	const Object::Scene &mScene;
	Trace::Tracer::Settings mSettings;
	Trace::Tracer::RenderData mRenderData;
	Listener *mListener;
	bool mRendering;
	DWORD mStartTime;
	CRITICAL_SECTION mCritSec;
	std::set<std::unique_ptr<Thread>> mThreads;
	std::set<std::unique_ptr<PrerenderThread>> mPrerenderThreads;
	int mBlocksStarted;
};

}

#endif
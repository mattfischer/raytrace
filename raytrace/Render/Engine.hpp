#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Forwards.hpp"
#include "Trace/Forwards.hpp"

#include "Render/Thread.hpp"
#include "Render/Framebuffer.hpp"
#include "Trace/Tracer.hpp"

#include <set>

#include <windows.h>

namespace Render {

class Engine : public Thread::Listener
{
public:
	class Listener {
	public:
		virtual void onRenderDone() = 0;
		virtual void onRenderStatus(const char *message) = 0;
	};

	Engine(const Object::Scene &scene);
	~Engine();

	bool rendering() const;

	void startRender(Framebuffer *framebuffer, Listener *listener);
	bool threadDone(Thread *thread);
	void setSettings(const Trace::Tracer::Settings &settings);

	Trace::Tracer createTracer();
	Trace::Tracer::Settings &settings();

private:
	void getBlock(int block, int &x, int &y, int &w, int &h);
	int widthInBlocks();
	int heightInBlocks();

	void beginPhase();
	void endPhase();

	const Object::Scene &mScene;
	Trace::Tracer::Settings mSettings;
	Trace::Tracer::RenderData mRenderData;
	Listener *mListener;
	Framebuffer *mFramebuffer;
	enum class State {
		Stopped,
		Prerender,
		Render
	};
	State mState;
	DWORD mStartTime;
	CRITICAL_SECTION mCritSec;
	std::set<std::unique_ptr<Thread>> mThreads;
	int mBlocksStarted;
};

}

#endif
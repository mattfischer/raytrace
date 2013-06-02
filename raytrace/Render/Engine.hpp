#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Forwards.hpp"
#include "Trace/Forwards.hpp"

#include "Render/Thread.hpp"

#include <windows.h>

namespace Render {

class Engine
{
public:
	class Listener {
	public:
		virtual void onRenderDone() = 0;
		virtual void onRenderStatus(const char *message) = 0;
	};

	Engine();
	~Engine();

	bool rendering() const;

	void startRender(Object::Scene *scene, const Trace::Tracer::Settings &settings, unsigned char *bits, Listener *listener);
	bool threadDone(Thread *thread);

private:
	Object::Scene *mScene;
	Trace::Tracer::Settings mSettings;
	unsigned char *mBits;
	Listener *mListener;
	bool mRendering;
	DWORD mStartTime;
	CRITICAL_SECTION mCritSec;
	Thread **mThreads;
	int mNumThreads;
	int mNumActiveThreads;
};

}

#endif
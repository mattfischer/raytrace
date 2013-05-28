#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Scene.hpp"
#include "Trace/Tracer.hpp"
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

	bool rendering() const;

	void startRender(Object::Scene *scene, const Trace::Tracer::Settings &settings, unsigned char *bits, Listener *listener);
	void threadDone(Thread *thread);

private:
	Object::Scene *mScene;
	Trace::Tracer::Settings mSettings;
	unsigned char *mBits;
	Listener *mListener;
	bool mRendering;
	DWORD mStartTime;
	LONG mNumThreads;
};

}

#endif
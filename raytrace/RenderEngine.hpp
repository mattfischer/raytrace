#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Scene.hpp"
#include "Trace/Tracer.hpp"

#include <windows.h>

class WorkerThread;
class RenderEngine
{
public:
	friend class WorkerThread;

	class Listener {
	public:
		virtual void onRenderDone() = 0;
		virtual void onRenderStatus(const char *message) = 0;
	};

	RenderEngine();

	bool rendering() const;
	Object::Scene *scene() const;
	const Trace::Tracer::Settings &settings() const;
	unsigned char *bits() const;

	void startRender(Object::Scene *scene, const Trace::Tracer::Settings &settings, unsigned char *bits, Listener *listener);

private:
	void threadDone(WorkerThread *thread);

	Object::Scene *mScene;
	Trace::Tracer::Settings mSettings;
	unsigned char *mBits;
	Listener *mListener;
	bool mRendering;
	DWORD mStartTime;
};

#endif
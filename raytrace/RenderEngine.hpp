#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Scene.hpp"
#include "Trace/Tracer.hpp"

class WorkerThread;
class RenderEngine
{
public:
	friend class WorkerThread;

	RenderEngine(Object::Scene *scene, const Trace::Tracer::Settings &settings);

	Object::Scene *scene() const;
	const Trace::Tracer::Settings &settings() const;
	unsigned char *bits() const;

	typedef void (*DoneCallback)(RenderEngine *, void*);

	void render(unsigned char *bits, DoneCallback doneCallback, void *data);

private:
	void threadDone(WorkerThread *thread);

	Object::Scene *mScene;
	Trace::Tracer::Settings mSettings;
	unsigned char *mBits;
	DoneCallback mDoneCallback;
	void *mData;
};

#endif
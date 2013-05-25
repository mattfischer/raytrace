#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Scene.hpp"
#include "Trace/Tracer.hpp"

class RenderEngine
{
public:
	RenderEngine(Object::Scene *scene, const Trace::Tracer::Settings &settings);

	Object::Scene *scene() const;
	const Trace::Tracer::Settings &settings() const;

	typedef void (*DoneCallback)(RenderEngine *, void*);

	void render(unsigned char *bits, DoneCallback doneCallback, void *data);

private:
	Object::Scene *mScene;
	Trace::Tracer::Settings mSettings;
};

#endif
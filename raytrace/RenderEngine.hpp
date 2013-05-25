#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Scene.hpp"
#include "Trace/Tracer.hpp"

class RenderEngine
{
public:
	RenderEngine(Object::Scene *scene, const Trace::Tracer::Settings &settings);

	typedef void (LineCallback)(unsigned char *, int, void*);

	void render(unsigned char *bits, LineCallback callback, void *data);

private:
	Object::Scene *mScene;
	Trace::Tracer::Settings mSettings;
};

#endif
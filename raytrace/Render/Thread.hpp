#ifndef THREAD_HPP
#define THREAD_HPP

#include "Object/Scene.hpp"
#include "Trace/Tracer.hpp"

namespace Render {

class Engine;
class Thread
{
public:
	Thread(Engine *engine, Object::Scene *scene, const Trace::Tracer::Settings &settings, unsigned char *bits);

	void start();

private:
	static void kickstart(void *data);

	void run();
	Object::Color antialiasPixel(float x, float y, float size, const Object::Color corners[4], int generation = 0) const;
	bool shouldAntialias(const Object::Color corners[4], float size) const;

	Engine *mEngine;
	Object::Scene *mScene;
	const Trace::Tracer::Settings &mSettings;
	unsigned char *mBits;
	Trace::Tracer mTracer;
};

}

#endif
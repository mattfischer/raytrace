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

	void start(int startLine, int numLines);

	int startLine();
	int numLines();

private:
	static void kickstart(void *data);

	struct SubPixel {
		bool valid;
		Object::Color color;
	};

	void run();
	void doRender();

	Object::Color antialiasPixel(float x, float y, int subpixelX, int subPixelY, int subPixelSize, const Object::Color corners[4]);
	bool shouldAntialias(const Object::Color corners[4]) const;
	SubPixel &subPixel(int x, int y);

	Engine *mEngine;
	Object::Scene *mScene;
	const Trace::Tracer::Settings &mSettings;
	unsigned char *mBits;
	int mStartLine;
	int mNumLines;
	Trace::Tracer mTracer;
	SubPixel *mSubPixels;
	int mSubPixelSize;
	bool mStarted;
};

}

#endif
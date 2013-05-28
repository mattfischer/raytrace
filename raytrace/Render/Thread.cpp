#include "Render/Thread.hpp"
#include "Render/Engine.hpp"

#include <process.h>

namespace Render {

Thread::Thread(Engine *engine, Object::Scene *scene, const Trace::Tracer::Settings &settings, unsigned char *bits)
: mSettings(settings),
  mTracer(scene, settings)
{
	mEngine = engine;
	mBits = bits;
}

void Thread::start(int startLine, int numLines)
{
	mStartLine = startLine;
	mNumLines = numLines;

	_beginthread(kickstart, 0, this);
}

void Thread::kickstart(void *data)
{
	Thread *obj = (Thread*)data;
	obj->run();
}

void Thread::run()
{
	int width = mSettings.width;
	int height = mSettings.height;
	for(int y=mStartLine; y<mStartLine + mNumLines; y++) {
		for(int x=0; x<width; x++) {
			Object::Color corners[4];
			corners[0] = mTracer.tracePixel(x, y);
			corners[1] = mTracer.tracePixel(x + 1, y);
			corners[2] = mTracer.tracePixel(x, y + 1);
			corners[3] = mTracer.tracePixel(x + 1, y + 1);

			Object::Color c = antialiasPixel(x + 0.5f, y + 0.5f, 1.0f, corners);
			int height = mSettings.height;
			int width = mSettings.width;
			int scany = height - y - 1;
			mBits[(scany * width + x) * 3 + 0] = c.blue() * 0xFF;
			mBits[(scany * width + x) * 3 + 1] = c.green() * 0xFF;
			mBits[(scany * width + x) * 3 + 2] = c.red() * 0xFF;
		}
	}

	mEngine->threadDone(this);
}

Object::Color Thread::antialiasPixel(float x, float y, float size, const Object::Color corners[4], int generation) const
{
	Object::Color ret;

	if(shouldAntialias(corners, size) && generation < mSettings.maxAAGen) {
		Object::Color subs[9];
		int idx = 0;
		for(int j=-1; j<=1; j++) {
			for(int i=-1; i<=1; i++) {
				switch(idx) {
					case 0: subs[idx] = corners[0]; break;
					case 2: subs[idx] = corners[1]; break;
					case 6: subs[idx] = corners[2]; break;
					case 8: subs[idx] = corners[3]; break;
					default: subs[idx] = mTracer.tracePixel(x + i * size / 2, y + j * size / 2); break;
				}
				idx++;
			}
		}

		Object::Color subpixels[4];
		Object::Color subcorners[4];

		subcorners[0] = subs[0]; subcorners[1] = subs[1]; subcorners[2] = subs[3]; subcorners[3] = subs[4];
		subpixels[0] = antialiasPixel(x - size/4, y - size/4, size / 2, subcorners, generation + 1);

		subcorners[0] = subs[1]; subcorners[1] = subs[2]; subcorners[2] = subs[4]; subcorners[3] = subs[5];
		subpixels[1] = antialiasPixel(x + size/4, y - size/4, size / 2, subcorners, generation + 1);

		subcorners[0] = subs[3]; subcorners[1] = subs[4]; subcorners[2] = subs[6]; subcorners[3] = subs[7];
		subpixels[2] = antialiasPixel(x - size/4, y + size/4, size / 2, subcorners, generation + 1);

		subcorners[0] = subs[4]; subcorners[1] = subs[5]; subcorners[2] = subs[7]; subcorners[3] = subs[8];
		subpixels[3] = antialiasPixel(x + size/4, y + size/4, size / 2, subcorners, generation + 1);

		for(int i=0; i<4; i++) {
			ret = ret + subpixels[i];
		}
		ret = ret / 4;
	} else {
		for(int i=0; i<4; i++) {
			ret = ret + corners[i];
		}
		ret = ret / 4;
	}

	return ret;
}

static float colorMag(const Object::Color &x, const Object::Color &y)
{
	float r = x.red() - y.red();
	float g = x.green() - y.green();
	float b = x.blue() - y.blue();

	return r * r + g * g + b * b;
}

bool Thread::shouldAntialias(const Object::Color corners[4], float size) const
{
	Object::Color center;
	float dist = 0;
	for(int i=0; i<4; i++) {
		center = center + corners[i];
	}
	center = center / 4;

	for(int i=0; i<4; i++) {
		dist += colorMag(center, corners[i]);
	}

	return dist > size * mSettings.threshold * mSettings.threshold;
}

}
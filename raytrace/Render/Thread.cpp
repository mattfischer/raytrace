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
	mStarted = false;
}

void Thread::start(int startLine, int numLines)
{
	mStartLine = startLine;
	mCurrentLine = startLine;
	mNumLines = numLines;

	if(!mStarted) {
		mStarted = true;
		_beginthread(kickstart, 0, this);
	}
}

void Thread::kickstart(void *data)
{
	Thread *obj = (Thread*)data;
	obj->run();
}

void Thread::run()
{
	while(true) {
		doRender();

		bool stop = mEngine->threadDone(this);
		if(stop) {
			break;
		}
	}
}

int Thread::startLine()
{
	return mStartLine;
}

int Thread::currentLine()
{
	return mCurrentLine;
}

int Thread::numLines()
{
	return mNumLines;
}

void Thread::setNumLines(int numLines)
{
	mNumLines = numLines;
}

void Thread::doRender()
{
	int width = mSettings.width;
	int height = mSettings.height;

	mSubPixelSize = 1 << mSettings.maxAAGen;
	mSubPixels = new SubPixel[(mSubPixelSize + 1) * (mSubPixelSize + 1)];

	SubPixel *topLine = new SubPixel[width * mSubPixelSize + 1];
	SubPixel *bottomLine = new SubPixel[width * mSubPixelSize + 1];

	for(int x=0; x<=width; x++) {
		topLine[x * mSubPixelSize].color = mTracer.tracePixel(x, mStartLine);
		topLine[x * mSubPixelSize].valid = true;

		if(x < width) {
			for(int i=0; i<mSubPixelSize; i++) {
				topLine[x * mSubPixelSize + i].valid = false;
			}
		}
	}

	for(int y=mStartLine+1; y<=mStartLine + mNumLines; y++) {
		for(int x=0; x<=width; x++) {
			mCurrentLine = y - 1;

			bottomLine[x * mSubPixelSize].color = mTracer.tracePixel(x, y);
			bottomLine[x * mSubPixelSize].valid = true;

			if(x == 0) {
				continue;
			}

			for(int i=1; i<mSubPixelSize; i++) {
				bottomLine[(x - 1) * mSubPixelSize + i].valid = false;
			}

			for(int j=0; j<=mSubPixelSize; j++) {
				if(x > 1) {
					subPixel(0, j) = subPixel(mSubPixelSize, j);
				} else {
					subPixel(0, j).valid = false;
				}
			}

			for(int i=1; i<=mSubPixelSize; i++) {
				subPixel(i, 0) = topLine[(x - 1) * mSubPixelSize + i];
				for(int j=1; j<=mSubPixelSize; j++) {
					subPixel(i, j).valid = false;
				}
			}

			Object::Color corners[4];
			corners[0] = topLine[(x-1) * mSubPixelSize].color;
			corners[1] = topLine[x * mSubPixelSize].color;
			corners[2] = bottomLine[(x-1) * mSubPixelSize].color;
			corners[3] = bottomLine[x * mSubPixelSize].color;

			subPixel(0, 0).color = corners[0];
			subPixel(0, 0).valid = true;
			subPixel(mSubPixelSize, 0).color = corners[1];
			subPixel(mSubPixelSize, 0).valid = true;
			subPixel(0, mSubPixelSize).color = corners[2];
			subPixel(0, mSubPixelSize).valid = true;
			subPixel(mSubPixelSize, mSubPixelSize).color = corners[3];
			subPixel(mSubPixelSize, mSubPixelSize).valid = true;

			Object::Color c = antialiasPixel(x - 0.5f, y - 0.5f, mSubPixelSize/2, mSubPixelSize/2, mSubPixelSize, corners);
			int height = mSettings.height;
			int width = mSettings.width;
			int scany = height - y;
			mBits[(scany * width + x - 1) * 3 + 0] = c.blue() * 0xFF;
			mBits[(scany * width + x - 1) * 3 + 1] = c.green() * 0xFF;
			mBits[(scany * width + x - 1) * 3 + 2] = c.red() * 0xFF;

			for(int i=0; i<mSubPixelSize; i++) {
				bottomLine[(x - 1) * mSubPixelSize + i] = subPixel(i, mSubPixelSize);
			}
		}

		SubPixel *temp = topLine;
		topLine = bottomLine;
		bottomLine = temp;
	}

	delete[] topLine;
	delete[] bottomLine;
	delete[] mSubPixels;
}

Object::Color Thread::antialiasPixel(float x, float y, int subPixelX, int subPixelY, int subPixelSize, const Object::Color corners[4])
{
	Object::Color ret;
	float size = (float) subPixelSize / (float) mSubPixelSize;

	if(shouldAntialias(corners) && subPixelSize > 2) {
		Object::Color subs[9];
		int idx = 0;
		for(int j=-1; j<=1; j++) {
			for(int i=-1; i<=1; i++) {
				int idx = (j + 1) * 3 + i + 1;

				if     (i == -1 && j == -1) subs[idx] = corners[0];
				else if(i == 1  && j == -1) subs[idx] = corners[1];
				else if(i == -1 && j ==  1) subs[idx] = corners[2];
				else if(i == 1  && j ==  1) subs[idx] = corners[3];
				else {
					int subX = subPixelX + i * subPixelSize / 2;
					int subY = subPixelY + j * subPixelSize / 2;
					if(!subPixel(subX, subY).valid) {
						subPixel(subX, subY).color = mTracer.tracePixel(x + i * size / 2, y + j * size / 2);
						subPixel(subX, subY).valid = true;
					}
					subs[idx] = subPixel(subX, subY).color;
				}
			}
		}

		Object::Color subpixels[4];
		Object::Color subcorners[4];

		for(int j=0; j<2; j++) {
			for(int i=0; i<2; i++) {
				int sIdx = j * 3 + i;
				subcorners[0] = subs[sIdx];
				subcorners[1] = subs[sIdx+1];
				subcorners[2] = subs[sIdx+3];
				subcorners[3] = subs[sIdx+4];

				int subSubPixelX = subPixelX + (i * 2 - 1) * subPixelSize / 4;
				int subSubPixelY = subPixelY + (j * 2 - 1) * subPixelSize / 4;
				float subX = x + (i * 2 - 1) * size/4;
				float subY = y + (j * 2 - 1) * size/4;
				subpixels[j * 2 + i] = antialiasPixel(subX, subY, subSubPixelX, subSubPixelY, subPixelSize / 2, subcorners);
			}
		}

		for(int i=0; i<4; i++) {
			ret = ret + subpixels[i];
		}
	} else {
		for(int i=0; i<4; i++) {
			ret = ret + corners[i];
		}
	}

	ret = ret / 4;

	return ret;
}

static float colorMag(const Object::Color &x, const Object::Color &y)
{
	float r = x.red() - y.red();
	float g = x.green() - y.green();
	float b = x.blue() - y.blue();

	return r * r + g * g + b * b;
}

bool Thread::shouldAntialias(const Object::Color corners[4]) const
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

	return dist > mSettings.threshold * mSettings.threshold;
}

Thread::SubPixel &Thread::subPixel(int x, int y)
{
	return mSubPixels[y * (mSubPixelSize + 1) + x];
}

}
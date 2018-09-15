#include "Render/Thread.hpp"
#include "Render/Engine.hpp"

#include <process.h>

namespace Render {

Thread::Thread(Listener *listener, Framebuffer *framebuffer)
{
	mListener = listener;
	mFramebuffer = framebuffer;
	mStarted = false;
}

void Thread::start(int startX, int startY, int width, int height)
{
	mStartX = startX;
	mStartY = startY;
	mWidth = width;
	mHeight = height;

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

		bool stop = mListener->threadDone(this);
		if(stop) {
			break;
		}
	}
}

void Thread::doRender()
{
	for (int y = mStartY; y < mStartY + mHeight; y++) {
		for (int x = mStartX; x < mStartX + mWidth; x++) {
			Object::Color color = renderPixel(x, y);
			mFramebuffer->setPixel(x, y, color);
		}
	}
}
}
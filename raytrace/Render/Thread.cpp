#include "Render/Thread.hpp"
#include "Render/Engine.hpp"

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
		mThread = std::thread([=] { run(); });
		mThread.detach();
	}
}

void Thread::run()
{
	while(true) {
		for (int y = mStartY; y < mStartY + mHeight; y++) {
			for (int x = mStartX; x < mStartX + mWidth; x++) {
				Object::Color color = renderPixel(x, y);
				mFramebuffer->setPixel(x, y, color);
			}
		}

		bool stop = mListener->threadDone(this);
		if(stop) {
			break;
		}
	}
}

}
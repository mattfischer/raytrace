#ifndef RENDER_THREAD_HPP
#define RENDER_THREAD_HPP

#include "Object/Forwards.hpp"
#include "Render/Forwards.hpp"

namespace Render {

class Engine;
class Thread
{
public:
	class Listener {
	public:
		virtual bool threadDone(Thread *thread) = 0;
	};

	Thread(Listener *listener, Framebuffer *framebuffer);

	void start(int startX, int startY, int width, int height);

protected:
	virtual Object::Color renderPixel(int x, int y) = 0;

private:
	static void kickstart(void *data);

	void run();
	void doRender();

	int mStartX;
	int mStartY;
	int mWidth;
	int mHeight;
	bool mStarted;
	Listener *mListener;
	Framebuffer *mFramebuffer;
};

}

#endif
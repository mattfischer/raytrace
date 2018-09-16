#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Forwards.hpp"
#include "Trace/Forwards.hpp"

#include "Render/Framebuffer.hpp"
#include "Trace/Tracer.hpp"

#include <set>
#include <thread>

#include <windows.h>

namespace Render {

class Engine
{
public:
	class Listener {
	public:
		virtual void onRenderDone() = 0;
		virtual void onRenderStatus(const char *message) = 0;
	};

	class Thread
	{
	public:
		Thread(Engine &engine);

		void start(int startX, int startY, int width, int height);

	protected:
		virtual Object::Color renderPixel(int x, int y) = 0;

	private:
		void run();

		int mStartX;
		int mStartY;
		int mWidth;
		int mHeight;
		bool mStarted;
		Engine &mEngine;
		std::thread mThread;
	};

	Engine(const Object::Scene &scene);

	bool rendering() const;

	void startRender(Listener *listener);
	bool threadDone(Thread *thread);
	void setSettings(const Trace::Tracer::Settings &settings);

	std::unique_ptr<Trace::Tracer> createTracer();
	Trace::Tracer::Settings &settings();
	Framebuffer &framebuffer();

private:
	void getBlock(int block, int &x, int &y, int &w, int &h);
	int widthInBlocks();
	int heightInBlocks();

	void beginPhase();
	void endPhase();

	const Object::Scene &mScene;
	Trace::Tracer::Settings mSettings;
	Trace::Tracer::RenderData mRenderData;
	Listener *mListener;
	std::unique_ptr<Framebuffer> mFramebuffer;
	enum class State {
		Stopped,
		Prerender,
		Render
	};
	State mState;
	DWORD mStartTime;
	std::mutex mMutex;
	std::set<std::unique_ptr<Thread>> mThreads;
	int mBlocksStarted;
};

}

#endif
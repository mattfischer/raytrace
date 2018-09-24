#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "Object/Color.hpp"
#include "Object/Scene.hpp"
#include "Object/Radiance.hpp"

#include "Render/Framebuffer.hpp"
#include "Render/Tracer.hpp"
#include "Lighter/Base.hpp"

#include <set>
#include <thread>
#include <mutex>
#include <random>

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

		Tracer &tracer();
		std::default_random_engine &randomEngine();

	private:
		void run();

		int mStartX;
		int mStartY;
		int mWidth;
		int mHeight;
		bool mStarted;
		Engine &mEngine;
		std::thread mThread;
		Tracer mTracer;
		std::default_random_engine mRandomEngine;
	};

	struct Settings
	{
		int width;
		int height;
		bool lighting;
		int maxRayGeneration;
		int antialiasSamples;
		bool radiantLighting;
		bool specularLighting;
		bool directLighting;
		int directSamples;
		bool indirectLighting;
		int indirectSamples;
		int indirectDirectSamples;
		bool irradianceCaching;
		float irradianceCacheThreshold;
	};

	Engine(const Object::Scene &scene);

	const Object::Scene &scene() const;

	bool rendering() const;

	void startRender(Listener *listener);
	bool threadDone(Thread *thread);
	void setSettings(const Settings &settings);

	const Settings &settings() const;
	Framebuffer &framebuffer();

	Object::Color toneMap(const Object::Radiance &radiance) const;
	Object::Radiance traceRay(const Math::Ray &ray, Render::Tracer &tracer) const;

private:
	void getBlock(int block, int &x, int &y, int &w, int &h);
	int widthInBlocks();
	int heightInBlocks();

	void beginPhase();
	void endPhase();

	Object::Color renderPixel(Thread &thread, int x, int y);

	const Object::Scene &mScene;
	Settings mSettings;
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
	std::vector<std::unique_ptr<Lighter::Base>> mLighters;
};

}

#endif
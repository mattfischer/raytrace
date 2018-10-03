#define NOMINMAX
#include "Render/Engine.hpp"

#include "Lighter/Direct.hpp"
#include "Lighter/Indirect.hpp"
#include "Lighter/Radiant.hpp"
#include "Lighter/Specular.hpp"
#include "Lighter/Utils.hpp"

#include <algorithm>
#include <memory>

namespace Render {

static const int BLOCK_SIZE = 64;

Engine::Thread::Thread(Engine &engine)
	: mEngine(engine)
	, mTracer(engine.scene(), engine.settings().width, engine.settings().height)
{
	mStarted = false;
}

void Engine::Thread::start(int startX, int startY, int width, int height)
{
	mStartX = startX;
	mStartY = startY;
	mWidth = width;
	mHeight = height;

	if (!mStarted) {
		mStarted = true;
		mThread = std::thread([=] { run(); });
		mThread.detach();
	}
}

void Engine::Thread::run()
{
	while (true) {
		for (int y = mStartY; y < mStartY + mHeight; y++) {
			for (int x = mStartX; x < mStartX + mWidth; x++) {
				Object::Color color = mEngine.doPixel(*this, x, y);
				mEngine.framebuffer().setPixel(x, y, color);
			}
		}

		bool stop = mEngine.threadDone(this);
		if (stop) {
			break;
		}
	}
}

Tracer &Engine::Thread::tracer()
{
	return mTracer;
}

std::default_random_engine &Engine::Thread::randomEngine()
{
	return mRandomEngine;
}

Engine::Engine(const Object::Scene &scene)
	: mScene(scene)
{
	mState = State::Stopped;
	mFramebuffer = std::make_unique<Framebuffer>(0, 0);
}

const Object::Scene &Engine::scene() const
{
	return mScene;
}

void Engine::startRender(Listener *listener)
{
	mListener = listener;
	mState = State::Prerender;
	mListener->onRenderStatus("");

	mStartTime = GetTickCount();

	mLighters.clear();
	if (mSettings.directLighting) {
		mLighters.push_back(std::make_unique<Lighter::Direct>(mSettings.directSamples));
	}

	if (mSettings.indirectLighting) {
		mLighters.push_back(std::make_unique<Lighter::Indirect>(mSettings.indirectSamples, mSettings.indirectDirectSamples, mSettings.irradianceCaching, mSettings.irradianceCacheThreshold));
	}

	if (mSettings.radiantLighting) {
		mLighters.push_back(std::make_unique<Lighter::Radiant>());
	}

	if (mSettings.specularLighting) {
		mLighters.push_back(std::make_unique<Lighter::Specular>(*this, mSettings.maxRayGeneration));
	}

	beginPhase();
}

bool Engine::rendering() const
{
	return mState != State::Stopped;
}

bool Engine::threadDone(Thread *doneThread)
{
	bool ret;

	std::lock_guard<std::mutex> guard(mMutex);

	if (mBlocksStarted < widthInBlocks() * heightInBlocks()) {
		int x, y;
		int w, h;

		getBlock(mBlocksStarted, x, y, w, h);
		doneThread->start(x, y, w, h);
		mBlocksStarted++;
		ret = false;
	}
	else {
		for (const std::unique_ptr<Thread> &thread : mThreads) {
			if (thread.get() == doneThread) {
				mThreads.erase(thread);
			}
		}

		if (mThreads.size() == 0) {
			endPhase();
		}
		ret = true;
	}

	return ret;
}

void Engine::getBlock(int block, int &x, int &y, int &w, int &h)
{
	int row = block / widthInBlocks();
	int col = block % widthInBlocks();

	x = col * BLOCK_SIZE;
	y = row * BLOCK_SIZE;
	w = std::min(BLOCK_SIZE, mSettings.width - x);
	h = std::min(BLOCK_SIZE, mSettings.height - y);
}

void Engine::beginPhase()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numThreads = sysinfo.dwNumberOfProcessors;
	mBlocksStarted = 0;
	for (int i = 0; i<numThreads; i++) {
		int x, y;
		int w, h;
		getBlock(mBlocksStarted, x, y, w, h);

		std::unique_ptr<Thread> thread = std::make_unique<Thread>(*this);
		thread->start(x, y, w, h);
		mThreads.insert(std::move(thread));
		mBlocksStarted++;
	}
}

void Engine::endPhase()
{
	switch (mState) {
	case State::Prerender:
		mState = State::Render;
		beginPhase();
		break;

	case State::Render:
	{
		mState = State::Stopped;

		DWORD endTime = GetTickCount();
		char buf[256];
		sprintf_s(buf, sizeof(buf), "Render time: %.3fs", (endTime - mStartTime) / 1000.0f);

		mListener->onRenderStatus(buf);
		mListener->onRenderDone();
		break;
	}

	default:
		break;
	}
}

int Engine::widthInBlocks()
{
	return (mSettings.width + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

int Engine::heightInBlocks()
{
	return (mSettings.height + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

const Engine::Settings &Engine::settings() const
{
	return mSettings;
}

void Engine::setSettings(const Settings &settings)
{
	if (settings.width != mSettings.width || settings.height != mSettings.height) {
		mFramebuffer = std::make_unique<Framebuffer>(settings.width, settings.height);
	}
	mSettings = settings;
}

Framebuffer &Engine::framebuffer()
{
	return *mFramebuffer;
}

Object::Color Engine::toneMap(const Object::Radiance &radiance) const
{
	float red = radiance.red() / (radiance.red() + 1);
	float green = radiance.green() / (radiance.green() + 1);
	float blue = radiance.blue() / (radiance.blue() + 1);

	return Object::Color(red, green, blue);
}

Object::Radiance Engine::traceRay(const Math::Ray &ray, Render::Tracer &tracer)  const
{
	Object::Intersection intersection = tracer.intersect(ray);

	Object::Radiance radiance;
	if (intersection.valid())
	{
		for (const std::unique_ptr<Lighter::Base> &lighter : mLighters) {
			radiance += lighter->light(intersection, tracer);
		}
	}

	return radiance;
}

Object::Color Engine::prerenderPixel(Thread &thread, int x, int y)
{
	Object::Color color;

	Math::Ray ray = thread.tracer().createCameraRay(x, y);
	Object::Intersection intersection = thread.tracer().intersect(ray);
	if (intersection.valid())
	{
		for (const std::unique_ptr<Lighter::Base> &lighter : mLighters) {
			if (lighter->prerender(intersection, thread.tracer())) {
				color = Object::Color(1, 1, 1);
			}
		}
	}

	return color;
}

Object::Color Engine::renderPixel(Thread &thread, int x, int y)
{
	Object::Color color;
	for (int u = 0; u < mSettings.antialiasSamples; u++) {
		for (int v = 0; v < mSettings.antialiasSamples; v++) {
			Math::Ray ray = thread.tracer().createCameraRay(x + (float)u / mSettings.antialiasSamples, y + (float)v / mSettings.antialiasSamples);

			if (mSettings.lighting) {
				Object::Radiance radiance = traceRay(ray, thread.tracer());
				color += toneMap(radiance);
			}
			else {
				Object::Intersection intersection = thread.tracer().intersect(ray);
				if (intersection.valid())
				{
					color += intersection.primitive()->surface().albedo().color(intersection.objectPoint());
				}
			}
		}
	}
	color = color / (mSettings.antialiasSamples * mSettings.antialiasSamples);

	return color;
}

Object::Color Engine::doPixel(Thread &thread, int x, int y)
{
	Object::Color color;

	switch (mState) {
		case State::Prerender:
			color = prerenderPixel(thread, x, y);
			break;

		case State::Render:
			color = renderPixel(thread, x, y);
			break;
	}

	return color;
}

}
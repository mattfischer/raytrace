#define NOMINMAX
#include "Render/RenderJob.hpp"
#include "Render/Engine.hpp"

#include "Object/Radiance.hpp"
#include "Math/Bivector.hpp"
#include "Render/Engine.hpp"

#include <algorithm>
#include <random>

namespace Render {
	const int MaxSamplesPerIteration = 100;

	RenderJob::RenderJob(const Object::Scene &scene, const Settings &settings, const Lighter::Master &lighter, Framebuffer &framebuffer)
		: TileJob(framebuffer)
		, mScene(scene)
		, mSettings(settings)
		, mLighter(lighter)
	{
		mPixelsDone.resize(framebuffer.width() * framebuffer.height());
		mTotalRadiance.resize(framebuffer.width() * framebuffer.height());
		mSamplerOffsets.resize(framebuffer.width() * framebuffer.height());

		std::uniform_int_distribution<unsigned int> dist(0, 256);
		std::default_random_engine engine;
		for (unsigned int &offset : mSamplerOffsets) {
			offset = dist(engine);
		}
		mNeedRepeat = false;
		mNumSamplesCompleted = 0;
		mNumSamplesThisIteration = 1;
	}

	std::unique_ptr<Job::ThreadLocal> RenderJob::createThreadLocal()
	{
		return std::make_unique<ThreadLocal>(10);
	}

	void RenderJob::renderPixel(int x, int y, Job::ThreadLocal &threadLocal)
	{
		int index = y * framebuffer().width() + x;
		if (mPixelsDone[index]) {
			return;
		}

		Sampler &sampler = static_cast<ThreadLocal&>(threadLocal).sampler;

		Object::Color color;
		Object::Color totalColor;
		bool pixelDone = false;

		const int runLength = 10;
		Object::Color colors[runLength];
		int colorIdx = 0;
		sampler.startSequence(mSamplerOffsets[index] + mNumSamplesCompleted);
		for (int sample = 0; sample < mNumSamplesThisIteration; sample++) {
			Math::Bivector dv;
			sampler.startSample();
			Math::Point2D imagePoint = Math::Point2D(x, y) + sampler.getValue2D();
			Math::Point2D aperturePoint = sampler.getValue2D();
			Math::Beam beam = mScene.camera().createPixelBeam(imagePoint, framebuffer().width(), framebuffer().height(), aperturePoint);
			Object::Intersection intersection = mScene.intersect(beam);
			int numSamples = mNumSamplesCompleted + sample + 1;

			if (intersection.valid())
			{
				if (mSettings.lighting) {
					mTotalRadiance[index] += mLighter.light(intersection, sampler, 0);
					color = Engine::toneMap(mTotalRadiance[index] / numSamples);
				}
				else {
					totalColor += intersection.albedo();
					color = totalColor / (sample + 1);
				}
			}

			colors[colorIdx] = color;
			colorIdx = (colorIdx + 1) % runLength;

			if (numSamples >= mSettings.maxSamples) {
				pixelDone = true;
				break;
			}

			/*if (numSamples > mSettings.minSamples) {
				float variance = 0;
				int numVarianceSamples = std::min(runLength, numSamples);
				for (int i = 0; i < numVarianceSamples; i++) {
					variance += (colors[i] - color).magnitude2() / numVarianceSamples;
				}
				if (variance < mSettings.sampleThreshold * mSettings.sampleThreshold) {
					pixelDone = true;
					break;
				}
			}*/
		}

		framebuffer().setPixel(x, y, color);

		if (pixelDone) {
			mPixelsDone[index] = true;
		}
		else {
			mNeedRepeat = true;
		}
	}

	bool RenderJob::needRepeat()
	{
		bool needRepeat = mNeedRepeat;
		if (mNeedRepeat) {
			mNeedRepeat = false;
			mNumSamplesCompleted += mNumSamplesThisIteration;
			mNumSamplesThisIteration *= 2;
			mNumSamplesThisIteration = std::min(mNumSamplesThisIteration, MaxSamplesPerIteration);
		}

		return needRepeat;
	}
}
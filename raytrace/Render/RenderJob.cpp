#define NOMINMAX
#include "Render/RenderJob.hpp"
#include "Render/Engine.hpp"

#include "Object/Radiance.hpp"
#include "Math/Bivector.hpp"
#include "Render/Engine.hpp"

#include <algorithm>

namespace Render {
	RenderJob::RenderJob(const Object::Scene &scene, const Settings &settings, const Lighter::Master &lighter, Framebuffer &framebuffer)
		: TileJob(framebuffer)
		, mScene(scene)
		, mSettings(settings)
		, mLighter(lighter)
	{
	}

	void RenderJob::renderPixel(int x, int y, Sampler &sampler)
	{
		Object::Color color;

		Object::Radiance totalRadiance;
		Object::Color totalColor;
		int numSamples = 0;

		const int runLength = 10;
		Object::Color colors[runLength];
		int colorIdx = 0;
		sampler.startSequence();
		while (true) {
			Math::Bivector dv;
			sampler.startSample();
			Math::Point2D imagePoint = Math::Point2D(x, y) + sampler.getValue2D();
			Math::Point2D aperturePoint = sampler.getValue2D();
			Math::Beam beam = mScene.camera().createPixelBeam(imagePoint, framebuffer().width(), framebuffer().height(), aperturePoint);
			Object::Intersection intersection = mScene.intersect(beam);
			numSamples++;

			if (intersection.valid())
			{
				if (mSettings.lighting) {
					totalRadiance += mLighter.light(intersection, sampler, 0);
					color = Engine::toneMap(totalRadiance / numSamples);
				}
				else {
					totalColor += intersection.albedo();
					color = totalColor / numSamples;
				}
			}

			colors[colorIdx] = color;
			colorIdx = (colorIdx + 1) % runLength;

			if (numSamples >= mSettings.maxSamples) {
				break;
			}

			if (numSamples > mSettings.minSamples) {
				float variance = 0;
				int numVarianceSamples = std::min(runLength, numSamples);
				for (int i = 0; i < numVarianceSamples; i++) {
					variance += (colors[i] - color).magnitude2() / numVarianceSamples;
				}
				if (variance < mSettings.sampleThreshold * mSettings.sampleThreshold) {
					break;
				}
			}
		}

		framebuffer().setPixel(x, y, color);
	}
}
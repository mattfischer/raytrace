#include "Lighter/Sampler.hpp"

namespace Lighter {
	Sampler::Sampler(int numSamples, std::default_random_engine &randomEngine)
		: mRandomEngine(randomEngine)
	{
		mStrataU = std::sqrt(numSamples);
		mStrataV = numSamples / mStrataU;
	}

	Math::Point2D Sampler::sample(int sample)
	{
		std::uniform_real_distribution<float> dist(0, 1);
		float u;
		float v;

		if (sample < mStrataU / mStrataV) {
			int stratumU = sample / mStrataV;
			int stratumV = sample % mStrataV;
			u = (stratumU + dist(mRandomEngine)) / mStrataU;
			v = (stratumV + dist(mRandomEngine)) / mStrataV;
		}
		else {
			u = dist(mRandomEngine);
			v = dist(mRandomEngine);
		}

		return Math::Point2D(u, v);
	}
}
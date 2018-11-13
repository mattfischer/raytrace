#include "Render/Sampler.hpp"

namespace Render {
	Sampler::Sampler(int numSamples, std::default_random_engine &randomEngine)
		: mRandomEngine(randomEngine)
	{
		mNumSamples = numSamples;
		mCurrentSample = 0;
		mStrataU = std::sqrt(numSamples);
		mStrataV = numSamples / mStrataU;
	}

	void Sampler::startSequence()
	{
		mCurrentSample = -1;
	}

	void Sampler::startSample()
	{
		mCurrentSample++;
	}

	Math::Point2D Sampler::getValue()
	{
		std::uniform_real_distribution<float> dist(0, 1);
		float u;
		float v;

		if (mCurrentSample < mStrataU / mStrataV) {
			int stratumU = mCurrentSample / mStrataV;
			int stratumV = mCurrentSample % mStrataV;
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
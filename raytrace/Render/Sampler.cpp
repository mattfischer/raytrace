#include "Render/Sampler.hpp"

#include <algorithm>

namespace Render {
	Sampler::Sampler(int numSamples, int numDimensions)
	{
		mNumDimensions = numDimensions;
		mNumSamples = numSamples;
		mCurrentSample = 0;
		mStrataU = std::sqrt(numSamples);
		mStrataV = numSamples / mStrataU;

		mPermutations.resize(mNumSamples * mNumDimensions);
		for (int i = 0; i < mNumDimensions; i++) {
			for (int j = 0; j < mNumSamples; j++) {
				mPermutations[i * mNumSamples + j] = j;
			}
			std::shuffle(mPermutations.begin() + i * mNumSamples, mPermutations.begin() + (i + 1) * mNumSamples, mRandomEngine);
		}
	}

	void Sampler::startSequence()
	{
		mCurrentSample = -1;
		mCurrentDimension = 0;
	}

	void Sampler::startSample()
	{
		mCurrentSample++;
		mCurrentDimension = 0;
	}

	Math::Point2D Sampler::getValue()
	{
		std::uniform_real_distribution<float> dist(0, 1);
		float u;
		float v;

		int sample;
		if (mCurrentDimension < mNumDimensions) {
			sample = mPermutations[mCurrentDimension * mNumSamples + mCurrentSample];
		}
		else {
			sample = mCurrentSample;
		}
		mCurrentDimension++;

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
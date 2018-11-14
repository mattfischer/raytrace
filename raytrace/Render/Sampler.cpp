#include "Render/Sampler.hpp"

#include <algorithm>
#include <tuple>

namespace Render {
	const int sPrimes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };

	Sampler::Sampler(unsigned int numDimensions)
	{
		mNumDimensions = std::min((unsigned int)numDimensions, sizeof(sPrimes) / sizeof(sPrimes[0]));
		mLastSamples.resize(mNumDimensions);
	}

	void Sampler::startSequence()
	{
		mCurrentDimension = 0;
		for (std::tuple<int, int> &sample : mLastSamples) {
			sample = std::make_tuple(0, 1);
		}
	}

	void Sampler::startSample()
	{
		mCurrentDimension = 0;
	}

	float Sampler::getValue()
	{
		if (mCurrentDimension >= mNumDimensions) {
			std::uniform_real_distribution<float> dist(0, 1);
			return dist(mRandomEngine);
		}

		int b = sPrimes[mCurrentDimension];

		int N;
		int D;
		std::tie(N, D) = mLastSamples[mCurrentDimension];

		int X = D - N;
		if (X == 1) {
			N = 1;
			D = b * D;
		}
		else {
			int Y = D / b;
			while (X <= Y) {
				Y = Y / b;
			}
			N = (b + 1) * Y - X;
		}

		mLastSamples[mCurrentDimension] = std::make_tuple(N, D);
		mCurrentDimension++;
		return (float)N / (float)D;
	}

	Math::Point2D Sampler::getValue2D()
	{
		return Math::Point2D(getValue(), getValue());
	}
}
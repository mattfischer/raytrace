#ifndef RENDER_SAMPLER_HPP
#define RENDER_SAMPLER_HPP

#include "Math/Point2D.hpp"

#include <random>

namespace Render {
	class Sampler {
	public:
		Sampler(int numSamples, int numDimensions);

		void startSequence();
		void startSample();

		Math::Point2D getValue();

	private:
		int mNumDimensions;
		int mNumSamples;
		int mCurrentSample;
		int mCurrentDimension;

		int mStrataU;
		int mStrataV;
		std::default_random_engine mRandomEngine;
		std::vector<int> mPermutations;
	};
}
#endif
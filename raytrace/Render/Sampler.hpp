#ifndef RENDER_SAMPLER_HPP
#define RENDER_SAMPLER_HPP

#include "Math/Point2D.hpp"

#include <random>

namespace Render {
	class Sampler {
	public:
		Sampler(int numSamples, std::default_random_engine &randomEngine);

		void startSequence();
		void startSample();

		Math::Point2D getValue();

	private:
		int mNumSamples;
		int mCurrentSample;

		int mStrataU;
		int mStrataV;
		std::default_random_engine &mRandomEngine;
	};
}
#endif
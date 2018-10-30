#ifndef LIGHTER_SAMPLER_HPP
#define LIGHTER_SAMPLER_HPP

#include "Math/Point2D.hpp"

#include <random>

namespace Lighter {
	class Sampler
	{
	public:
		Sampler(int numSamples, std::default_random_engine &randomEngine);

		Math::Point2D sample(int sample);

	private:
		int mStrataU;
		int mStrataV;
		std::default_random_engine &mRandomEngine;
	};
}
#endif
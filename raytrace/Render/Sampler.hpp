#ifndef RENDER_SAMPLER_HPP
#define RENDER_SAMPLER_HPP

#include "Math/Point2D.hpp"

#include <vector>
#include <random>

namespace Render {
	class Sampler {
	public:
		Sampler(unsigned int numDimensions);

		void startSequence();
		void startSample();

		float getValue();
		Math::Point2D getValue2D();

	private:
		unsigned int mNumDimensions;
		unsigned int mCurrentDimension;
		std::default_random_engine mRandomEngine;

		std::vector<std::tuple<int, int>> mLastSamples;
	};
}
#endif
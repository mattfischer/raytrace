#ifndef LIGHTER_SAMPLER_HPP
#define LIGHTER_SAMPLER_HPP

#include <random>

namespace Lighter {
	class Sampler
	{
	public:
		Sampler(int numSamples, std::default_random_engine &randomEngine);

		void sample(int sample, float &u, float &v);

	private:
		int mStrataU;
		int mStrataV;
		std::default_random_engine &mRandomEngine;
	};
}
#endif
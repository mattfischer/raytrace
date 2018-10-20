#ifndef LIGHTER_UTILS_HPP
#define LIGHTER_UTILS_HPP

#include "Math/Vector.hpp"

#include <random>

namespace Lighter {
	class Utils {
	public:
		static Math::Vector sampleHemisphereCosineWeighted(int i, int N, std::default_random_engine &engine);
		static Math::Vector sampleHemisphere(int i, int N, float maxAngle, std::default_random_engine &engine);
		static void stratifiedSamples(int i, int N, float &u, float &v, std::default_random_engine &engine);
	};
}

#endif
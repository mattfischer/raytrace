#define _USE_MATH_DEFINES
#include "Lighter/Utils.hpp"

#include <random>
#include <cmath>

namespace Lighter {
	Math::Vector Utils::sampleHemisphereCosineWeighted(int i, int N, std::default_random_engine &engine)
	{
		float u, v;
		stratifiedSamples(i, N, u, v, engine);
	
		float phi = v * 2 * M_PI;
		float r = std::sqrt(u);

		return Math::Vector(r * std::cos(phi), r * std::sin(phi), std::sqrt(1 - u));
	}

	Math::Vector Utils::sampleHemisphere(int i, int N, float maxAngle, std::default_random_engine &engine)
	{
		float u, v;
		stratifiedSamples(i, N, u, v, engine);

		float phi = v * 2 * M_PI;
		float theta = std::acos(1 - u * (1 - std::cos(maxAngle)));
		float r = std::sin(theta);

		return Math::Vector(r * std::cos(phi), r * std::sin(phi), std::sqrt(1 - r * r));
	}

	void Utils::stratifiedSamples(int i, int N, float &u, float &v, std::default_random_engine &engine)
	{
		int A = sqrt(N);
		int B = N / A;
		std::uniform_real_distribution<float> dist(0, 1);

		if (i < A * B) {
			int a = i / B;
			int b = i % B;
			u = (a + dist(engine)) / A;
			v = (b + dist(engine)) / B;
		}
		else {
			u = dist(engine);
			v = dist(engine);
		}
	}
}

#define _USE_MATH_DEFINES
#include "Trace/Lighter/Utils.hpp"

#include <random>
#include <cmath>

namespace Trace {
namespace Lighter {

void Utils::orthonormalBasis(const Math::Vector &n, Math::Vector &x, Math::Vector &y)
{
	x = Math::Vector();
	y = Math::Vector();

	Math::Vector vectors[] = { Math::Vector(1,0,0), Math::Vector(0,1,0), Math::Vector(0,0,1) };

	for (const Math::Vector &v : vectors) {
		Math::Vector p = v - n * (v * n);
		if (p.magnitude2() > x.magnitude2()) {
			y = x;
			x = p;
		}
		else if (p.magnitude2() > y.magnitude2()) {
			y = p;
		}
	}

	x = x.normalize();
	y = y - x * (x * y);

	y = y.normalize();
}

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
		int a = i / A;
		int b = i % A;
		u = (a + dist(engine)) / A;
		v = (b + dist(engine)) / B;
	}
	else {
		u = dist(engine);
		v = dist(engine);
	}
}

}
}
#ifndef MATH_BIVECTOR_2D_HPP
#define MATH_BIVECTOR_2D_HPP

#include "Math/Vector2D.hpp"

namespace Math {
	class Bivector2D
	{
	public:
		Bivector2D() = default;
		Bivector2D(const Math::Vector2D &u, const Math::Vector2D &v);

		const Math::Vector2D &u() const;
		const Math::Vector2D &v() const;

		Bivector2D operator*(float b) const;
		Bivector2D operator+(const Bivector2D &b) const;
		Bivector2D operator-(const Bivector2D &b) const;
		Bivector2D operator/(float b) const;

		Math::Vector2D operator*(const Math::Vector2D &b) const;

	private:
		Math::Vector2D mU;
		Math::Vector2D mV;
	};
}
#endif
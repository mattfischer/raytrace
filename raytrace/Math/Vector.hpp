#ifndef MATH_VECTOR_HPP
#define MATH_VECTOR_HPP

#include "Math/Coordinate.hpp"

namespace Math {
	class Point;
	class Normal;
	class BaseTransformation;
	class Vector : public Coordinate
	{
	public:
		Vector();
		Vector(float x, float y, float z);
		explicit Vector(const Point &point);
		explicit Vector(const Coordinate &c);

		Vector normalize() const;
		float magnitude() const;
		float magnitude2() const;

		Vector operator+(const Vector &b) const;
		Vector operator-(const Vector &b) const;
		float operator*(const Vector &b) const;
		Vector operator*(float b) const;
		Vector operator/(float b) const;
		Vector operator-() const;
		Vector operator%(const Vector &b) const;

		static Vector fromPolar(float phi, float theta, float r);
	};

	Vector operator*(const BaseTransformation &transformation, const Vector &vector);
}

#endif

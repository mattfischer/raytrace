#include "Math/Vector.hpp"
#include "Math/Point.hpp"
#include "Math/Transformation.hpp"
#include "Math/Normal.hpp"

#include <cmath>

namespace Math {
	Vector::Vector()
	{
	}

	Vector::Vector(float x, float y, float z)
	: Coordinate(x, y, z, 0)
	{
	}

	Vector::Vector(const Coordinate &c)
	: Coordinate(c)
	{
	}

	Vector::Vector(const Point &point)
	: Coordinate(point.x(), point.y(), point.z(), 0)
	{
	}

	float Vector::magnitude() const
	{
		return std::sqrt(magnitude2());
	}

	float Vector::magnitude2() const
	{
		return *this * *this;
	}

	Vector Vector::normalize() const
	{
		float m = magnitude();

		return Vector(x() / m, y() / m, z() / m);
	}

	Vector Vector::operator+(const Vector &b) const
	{
		return Vector(x() + b.x(), y() + b.y(), z() + b.z());
	}

	Vector Vector::operator-(const Vector &b) const
	{
		return Vector(x() - b.x(), y() - b.y(), z() - b.z());
	}

	float Vector::operator*(const Vector &b) const
	{
		return x() * b.x() + y() * b.y() + z() * b.z();
	}

	Vector Vector::operator*(float b) const
	{
		return Vector(x() * b, y() * b, z() * b);
	}

	Vector Vector::operator/(float b) const
	{
		return Vector(x() / b, y() / b, z() / b);
	}

	Vector Vector::operator-() const
	{
		return *this * -1;
	}

	Vector Vector::operator%(const Math::Vector &b) const
	{
		return Vector(y() * b.z() - z() * b.y(), z() * b.x() - x() * b.z(), x() * b.y() - y() * b.x());
	}

	Vector Vector::fromPolar(float phi, float theta, float r)
	{
		return Vector(r * std::cos(phi) * std::cos(theta), r * std::sin(phi) * std::cos(theta), r * std::sin(theta));
	}

	Vector operator*(const BaseTransformation &transformation, const Vector &vector)
	{
		return Vector(transformation.matrix() * vector);
	}
}
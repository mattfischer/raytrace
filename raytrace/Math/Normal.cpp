#include "Math/Normal.hpp"
#include "Math/Point.hpp"
#include "Math/Transformation.hpp"
#include "Math/Vector.hpp"

#include <cmath>

namespace Math {

	Normal::Normal()
	{
	}

	Normal::Normal(float x, float y, float z)
	: Coordinate(x, y, z, 0)
	{
	}

	Normal::Normal(const ASTVector &astVector)
	: Coordinate(astVector, 0)
	{
	}

	Normal::Normal(const Normal &c)
	: Coordinate(c)
	{
	}

	Normal::Normal(const Coordinate &c)
	: Coordinate(c)
	{
	}

	Normal::Normal(const Point &point)
	: Coordinate(point.x(), point.y(), point.z(), 0)
	{
	}

	float Normal::magnitude() const
	{
		return std::sqrt(magnitude2());
	}

	float Normal::magnitude2() const
	{
		return *this * *this;
	}

	Normal Normal::normalize() const
	{
		float m = magnitude();

		return Normal(x() / m, y() / m, z() / m);
	}

	Normal Normal::operator+(const Normal &b) const
	{
		return Normal(x() + b.x(), y() + b.y(), z() + b.z());
	}

	float Normal::operator*(const Normal &b) const
	{
		return x() * b.x() + y() * b.y() + z() * b.z();
	}

	Normal operator*(const BaseTransformation &transformation, const Normal &normal)
	{
		return Normal(normal * transformation.inverseMatrix());
	}

	Normal Normal::operator-() const
	{
		return Normal(-x(), -y(), -z());
	}

	float Normal::operator*(const Vector &b) const
	{
		return x() * b.x() + y() * b.y() + z() * b.z();
	}

	Normal Normal::operator*(float b) const
	{
		return Normal(x() * b, y() * b, z() * b);
	}

	Normal Normal::operator/(float b) const
	{
		return Normal(x() / b, y() / b, z() / b);
	}

	Normal Normal::operator%(const Math::Normal &b) const
	{
		return Normal(y() * b.z() - z() * b.y(), z() * b.x() - x() * b.z(), x() * b.y() - y() * b.x());
	}

	float operator*(const Vector &vector, const Normal &normal)
	{
		return normal * vector;
	}
}
#include "Math/Vector.hpp"
#include "Math/Point.hpp"
#include "Math/Transformation.hpp"
#include "Math/Normal.hpp"

#include <cmath>

namespace Math {
	Vector::Vector()
	{
		mX = mY = mZ = 0;
	}

	Vector::Vector(float x, float y, float z)
	{
		mX = x;
		mY = y;
		mZ = z;
	}

	Vector::Vector(const Point &point)
	{
		mX = point.x();
		mY = point.y();
		mZ = point.z();
	}

	Vector::Vector(const Normal &normal)
	{
		mX = normal.x();
		mY = normal.y();
		mZ = normal.z();
	}

	float Vector::x() const
	{
		return mX;
	}

	float Vector::y() const
	{
		return mY;
	}

	float Vector::z() const
	{
		return mZ;
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

	Vector operator*(const Matrix &matrix, const Vector &vector)
	{
		if (matrix.identity()) return vector;

		float x = matrix(0, 0) * vector.x() + matrix(1, 0) * vector.y() + matrix(2, 0) * vector.z();
		float y = matrix(0, 1) * vector.x() + matrix(1, 1) * vector.y() + matrix(2, 1) * vector.z();
		float z = matrix(0, 2) * vector.x() + matrix(1, 2) * vector.y() + matrix(2, 2) * vector.z();

		return Vector(x, y, z);
	}

	Vector operator*(const Vector &vector, const Matrix &matrix)
	{
		if (matrix.identity()) return vector;

		float x = matrix(0, 0) * vector.x() + matrix(0, 1) * vector.y() + matrix(0, 2) * vector.z();
		float y = matrix(1, 0) * vector.x() + matrix(1, 1) * vector.y() + matrix(1, 2) * vector.z();
		float z = matrix(2, 0) * vector.x() + matrix(2, 1) * vector.y() + matrix(2, 2) * vector.z();

		return Vector(x, y, z);
	}
}
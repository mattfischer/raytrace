#include "Math/Vector.hpp"
#include "Math/Point.hpp"

namespace Math {

Vector::Vector(const Point &point)
: Coordinate(point)
{
}

double Vector::magnitude() const
{
	return sqrt(magnitude2());
}

double Vector::magnitude2() const
{
	return *this * *this;
}

Vector Vector::normalize() const
{
	double m = magnitude();

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

double Vector::operator*(const Vector &b) const
{
	return x() * b.x() + y() * b.y() + z() * b.z();
}

Vector Vector::operator*(double b) const
{
	return Vector(x() * b, y() * b, z() * b);
}

Vector Vector::operator/(double b) const
{
	return Vector(x() / b, y() / b, z() / b);
}

Vector Vector::operator-() const
{
	return *this * -1;
}

}
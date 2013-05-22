#include "Math/Point.hpp"

namespace Math {
	
Point Point::operator+(const Vector &b) const
{
	return Point(x() + b.x(), y() + b.y(), z() + b.z());
}

Point Point::operator-(const Vector &b) const
{
	return Point(x() - b.x(), y() - b.y(), z() - b.z());
}

Vector Point::operator-(const Point &b) const
{
	return Vector(x() - b.x(), y() - b.y(), z() - b.z());
}

}
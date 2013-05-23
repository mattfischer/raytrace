#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"

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

Point operator*(const Transformation &transformation, const Point &point)
{
	return Point(transformation.matrix() * point);
}

}
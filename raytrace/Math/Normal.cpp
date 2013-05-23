#include "Math/Normal.hpp"
#include "Math/Point.hpp"
#include "Math/Transformation.hpp"
#include "Math/Vector.hpp"

namespace Math {

Normal::Normal(const Point &point)
: Coordinate(point.x(), point.y(), point.z(), 0)
{
}

float Normal::operator*(const Normal &b) const
{
	return x() * b.x() + y() * b.y() + z() * b.z();
}

Normal operator*(const Transformation &transformation, const Normal &normal)
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

float operator*(const Vector &vector, const Normal &normal)
{
	return normal * vector;
}

}
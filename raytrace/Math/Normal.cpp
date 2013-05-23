#include "Math/Normal.hpp"
#include "Math/Point.hpp"
#include "Math/Transformation.hpp"
#include "Math/Vector.hpp"

namespace Math {

Normal::Normal(const Point &point)
: Coordinate(point.x(), point.y(), point.z(), 0)
{
}

double Normal::operator*(const Normal &b) const
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

double operator*(const Normal &normal, const Vector &vector)
{
	return normal.x() * vector.x() + normal.y() * vector.y() + normal.z() * vector.z();
}

double operator*(const Vector &vector, const Normal &normal)
{
	return normal * vector;
}

}
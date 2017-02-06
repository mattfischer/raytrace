#include "Trace/Ray.hpp"

namespace Trace {

Ray::Ray()
{
}

Ray::Ray(const Math::Point &origin, const Math::Vector &direction)
: mOrigin(origin), mDirection(direction)
{
}

Ray::Ray(const Ray &c)
: mOrigin(c.origin()), mDirection(c.direction())
{
}

Ray &Ray::operator=(const Ray &c)
{
	mOrigin = c.origin();
	mDirection = c.direction();

	return *this;
}

Ray Ray::createFromPoints(const Math::Point &point1, const Math::Point &point2)
{
	return Ray(point1, (point2 - point1).normalize());
}

Ray operator*(const Math::BaseTransformation &transformation, const Ray &ray)
{
	return Ray(transformation * ray.origin(), transformation * ray.direction());
}

const Math::Point &Ray::origin() const
{
	return mOrigin;
}

const Math::Vector &Ray::direction() const
{
	return mDirection;
}

}
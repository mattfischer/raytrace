#include "Trace/Ray.hpp"

namespace Trace {

Ray::Ray()
{
	mGeneration = 1;
}

Ray::Ray(const Math::Point &origin, const Math::Vector &direction)
: mOrigin(origin), mDirection(direction), mGeneration(1)
{
}

Ray::Ray(const Ray &c)
: mOrigin(c.origin()), mDirection(c.direction()), mGeneration(c.generation())
{
}

Ray &Ray::operator=(const Ray &c)
{
	mOrigin = c.origin();
	mDirection = c.direction();
	mGeneration = c.generation();

	return *this;
}

Ray Ray::createFromPoints(const Math::Point &point1, const Math::Point &point2)
{
	return Ray(point1, (point2 - point1).normalize());
}

Ray operator*(const Math::BaseTransformation &transformation, const Ray &ray)
{
	return Ray(transformation * ray.origin(), transformation * ray.direction() - transformation * Math::Vector(0, 0, 0));
}

}
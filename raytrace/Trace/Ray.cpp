#include "Trace/Ray.hpp"

namespace Trace {

Ray::Ray()
{
	mGeneration = 1;
}

Ray::Ray(const Math::Point &origin, const Math::Vector &direction, int generation)
: mOrigin(origin), mDirection(direction), mGeneration(generation)
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

Ray Ray::createFromPoints(const Math::Point &point1, const Math::Point &point2, int generation)
{
	return Ray(point1, (point2 - point1).normalize(), generation);
}

Ray operator*(const Math::BaseTransformation &transformation, const Ray &ray)
{
	return Ray(transformation * ray.origin(), transformation * ray.direction() - transformation * Math::Vector(0, 0, 0), ray.generation());
}

const Math::Point &Ray::origin() const
{
	return mOrigin;
}

const Math::Vector &Ray::direction() const
{
	return mDirection;
}

int Ray::generation() const
{
	return mGeneration;
}

}
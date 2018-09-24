#include "Math/Ray.hpp"

namespace Math {

Ray::Ray()
{
}

Ray::Ray(const Point &origin, const Vector &direction, int generation)
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

Ray Ray::createFromPoints(const Point &point1, const Point &point2, int generation)
{
	return Ray(point1, (point2 - point1).normalize(), generation);
}

Ray operator*(const BaseTransformation &transformation, const Ray &ray)
{
	return Ray(transformation * ray.origin(), transformation * ray.direction(), ray.generation());
}

const Point &Ray::origin() const
{
	return mOrigin;
}

const Vector &Ray::direction() const
{
	return mDirection;
}

int Ray::generation() const
{
	return mGeneration;
}

}
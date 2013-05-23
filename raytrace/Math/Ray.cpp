#include "Math/Ray.hpp"

namespace Math {

Ray::Ray()
{
	mGeneration = 1;
}

Ray::Ray(const Point &origin, const Vector &direction)
{
	mOrigin = origin;
	mDirection = direction;
	mGeneration = 1;
}

Ray::Ray(const Ray &c)
{
	mOrigin = c.origin();
	mDirection = c.direction();
	mGeneration = c.generation();
}

Ray &Ray::operator=(const Ray &c)
{
	mOrigin = c.origin();
	mDirection = c.direction();
	mGeneration = c.generation();

	return *this;
}

Ray Ray::createFromPoints(const Point &point1, const Point &point2)
{
	return Ray(point1, (point2 - point1).normalize());
}

void Ray::setOrigin(const Point &origin)
{
	mOrigin = origin;
}

void Ray::setDirection(const Vector &direction)
{
	mDirection = direction;
}

int Ray::generation() const
{
	return mGeneration;
}

void Ray::setGeneration(int generation)
{
	mGeneration = generation;
}

Ray operator*(const Transformation &transformation, const Ray &ray)
{
	return Ray(transformation * ray.origin(), transformation * ray.direction() - transformation * Vector(0, 0, 0));
}

}
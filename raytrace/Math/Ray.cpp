#include "Math/Ray.hpp"

namespace Math {

Ray::Ray()
{
	mGeneration = 1;
}

Ray::Ray(const Vector &origin, const Vector &direction)
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

Ray Ray::createFromPoints(const Vector &point1, const Vector &point2)
{
	return Ray(point1, (point2 - point1).normalize());
}

void Ray::setOrigin(const Vector &origin)
{
	mOrigin = origin;
}

void Ray::setDirection(const Vector &direction)
{
	mDirection = direction;
}

Ray Ray::transform(const Matrix &transformation) const 
{
	return Ray(transformation * mOrigin, transformation * mDirection - transformation * Vector(0, 0, 0));
}

int Ray::generation() const
{
	return mGeneration;
}

void Ray::setGeneration(int generation)
{
	mGeneration = generation;
}

}
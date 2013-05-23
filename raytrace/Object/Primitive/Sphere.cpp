#include "Object/Primitive/Sphere.hpp"
#include "Math/Transformation.hpp"

#include <math.h>

#define EPSILON 0.0001

namespace Object {
namespace Primitive {

Sphere::Sphere()
{
}

Sphere *Sphere::fromAst(AST *ast)
{
	return new Sphere();
}

void Sphere::doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const
{
	Trace::Intersection intersection;
	double a, b, c;
	double disc;

	a = ray.direction().magnitude2();
	b = 2 * (Math::Vector(ray.origin()) * ray.direction());
	c = Math::Vector(ray.origin()).magnitude2() - 1;

	disc = b * b - 4 * a * c;
	if(disc >= 0)
	{
		double distance = (-b - sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			intersections.push_back(Trace::Intersection(this, distance, Math::Normal(point), point));
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			intersections.push_back(Trace::Intersection(this, distance, Math::Normal(point), point));
		}
	}
}

bool Sphere::doInside(const Math::Point &point) const
{
	return Math::Vector(point).magnitude2() <= 1;
}

}
}
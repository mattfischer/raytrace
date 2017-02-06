#include "Object/Primitive/Sphere.hpp"
#include "Math/Transformation.hpp"

#include <math.h>

#define EPSILON 0.0001

namespace Object {
namespace Primitive {

std::unique_ptr<Sphere> Sphere::fromAst(AST *ast)
{
	return std::make_unique<Sphere>();
}

void Sphere::doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
{
	Trace::Intersection intersection;
	float a, b, c;
	float disc;

	a = ray.direction().magnitude2();
	b = 2 * (Math::Vector(ray.origin()) * ray.direction());
	c = Math::Vector(ray.origin()).magnitude2() - 1;

	disc = b * b - 4 * a * c;
	if(disc >= 0)
	{
		float distance = (-b - sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			intersections.push_back(Trace::Intersection(this, ray, distance, Math::Normal(point), point));
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			intersections.push_back(Trace::Intersection(this, ray, distance, Math::Normal(point), point));
		}
	}
}

bool Sphere::doInside(const Math::Point &point) const
{
	return Math::Vector(point).magnitude2() <= 1;
}

}
}
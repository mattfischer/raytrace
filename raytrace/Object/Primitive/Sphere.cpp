#include "Object/Primitive/Sphere.hpp"
#include "Math/Transformation.hpp"

#include <math.h>

#define EPSILON 0.001

namespace Object {
namespace Primitive {

std::unique_ptr<Sphere> Sphere::fromAst(AST *ast)
{
	std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
	Math::Vector position(ast->children[0]->children[0]->data._vector);
	float radius = ast->children[0]->children[1]->data._float;
	sphere->transform(Math::Transformation::translate(position));
	sphere->transform(Math::Transformation::scale(radius, radius, radius));

	parseAstCommon(*sphere, ast->children[1]);

	return sphere;
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

BoundingSphere Sphere::doBoundingSphere() const
{
	return BoundingSphere(Math::Point(0, 0, 0), 1);
}

}
}
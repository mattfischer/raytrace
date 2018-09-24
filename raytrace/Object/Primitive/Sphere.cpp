#include "Object/Primitive/Sphere.hpp"
#include "Math/Transformation.hpp"

#include <math.h>

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

Intersection Sphere::doIntersect(const Math::Ray &ray) const
{
	float a, b, c;
	float disc;

	a = ray.direction().magnitude2();
	b = 2 * (Math::Vector(ray.origin()) * ray.direction());
	c = Math::Vector(ray.origin()).magnitude2() - 1;

	disc = b * b - 4 * a * c;
	if(disc >= 0)
	{
		float distance = (-b - sqrt(disc)) / (2 * a);

		if(distance > 0)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			return Intersection(this, ray, distance, Math::Normal(point), point);
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance > 0)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			return Intersection(this, ray, distance, Math::Normal(point), point);
		}
	}

	return Intersection();
}

bool Sphere::doInside(const Math::Point &point) const
{
	return Math::Vector(point).magnitude2() <= 1;
}

BoundingVolume Sphere::doBoundingVolume(const std::vector<Math::Vector> &vectors) const
{
	std::vector<float> mins;
	std::vector<float> maxes;

	for (const Math::Vector &vector : vectors) {
		mins.push_back(-1 / vector.magnitude());
		maxes.push_back(1 / vector.magnitude());
	}

	return BoundingVolume(mins, maxes);
}

}
}
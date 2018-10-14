#include "Object/Primitive/Sphere.hpp"
#include "Math/Transformation.hpp"

#include <math.h>

namespace Object {
namespace Primitive {

std::unique_ptr<Sphere> Sphere::fromAst(AST *ast)
{
	Math::Point position(ast->children[0]->children[0]->data._vector);
	float radius = ast->children[0]->children[1]->data._float;
	std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>(position, radius);

	parseAstCommon(*sphere, ast->children[1]);

	return sphere;
}

Sphere::Sphere(const Math::Point &position, float radius)
	: mPosition(position), mRadius(radius)
{
}

float Sphere::doIntersect(const Math::Ray &ray, Math::Normal &normal) const
{
	float a, b, c;
	float disc;

	a = ray.direction().magnitude2();
	b = 2 * ((ray.origin() - mPosition) * ray.direction());
	c = (ray.origin() - mPosition).magnitude2() - mRadius * mRadius;

	disc = b * b - 4 * a * c;
	if(disc >= 0)
	{
		float distance = (-b - sqrt(disc)) / (2 * a);

		if(distance >= 0)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			normal = Math::Normal(point - mPosition) / mRadius;
			return distance;
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance >= 0)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			normal = Math::Normal(point - mPosition) / mRadius;
			return distance;
		}
	}

	return FLT_MAX;
}

bool Sphere::doInside(const Math::Point &point) const
{
	return Math::Vector(point).magnitude2() <= 1;
}

BoundingVolume Sphere::doBoundingVolume() const
{
	float mins[BoundingVolume::NUM_VECTORS];
	float maxes[BoundingVolume::NUM_VECTORS];

	for(int i=0; i<BoundingVolume::NUM_VECTORS; i++) {
		const Math::Vector &vector = BoundingVolume::vectors()[i];
		float x = Math::Vector(mPosition) * vector;
		float y = mRadius * vector.magnitude();

		mins[i] = x - y;
		maxes[i] = x + y;
	}

	return BoundingVolume(mins, maxes);
}

}
}
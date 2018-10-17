#include "Object/Primitive/Box.hpp"

#include <math.h>
#include <algorithm>

namespace Object {
namespace Primitive {

std::unique_ptr<Box> Box::fromAst(AST *ast)
{
	std::unique_ptr<Box> box = std::make_unique<Box>();
	Math::Vector position(ast->children[0]->children[0]->data._vector);
	Math::Vector size(ast->children[0]->children[1]->data._vector);

	box->transform(Math::Transformation::translate(position + size / 2));
	box->transform(Math::Transformation::scale(size / 2));

	parseAstCommon(*box, ast->children[1]);

	return box;
}

void Box::testIntersect(const Math::Ray &ray, const Math::Normal &normal, float &outDistance, Math::Normal &outNormal) const
{
	float distance = (Math::Vector(ray.origin()) * normal - 1) / (ray.direction() * -normal);
	if (distance > 0)
	{
		Math::Point point = ray.origin() + ray.direction() * distance;
		point = point - Math::Vector(normal) * (Math::Vector(point) * normal - 1);

		if(abs(point.x()) <= 1 && abs(point.y()) <= 1 && abs(point.z()) <= 1)
    	{
			if (distance < outDistance) {
				outDistance = distance;
				outNormal = normal;
			}
		}
	}
}

float Box::doIntersect(const Math::Ray &ray, Math::Normal &normal) const
{
	float distance = FLT_MAX;

	testIntersect(ray, Math::Normal(1,0,0), distance, normal);
	testIntersect(ray, Math::Normal(-1,0,0), distance, normal);
	testIntersect(ray, Math::Normal(0,1,0), distance, normal);
	testIntersect(ray, Math::Normal(0,-1,0), distance, normal);
	testIntersect(ray, Math::Normal(0,0,1), distance, normal);
	testIntersect(ray, Math::Normal(0,0,-1), distance, normal);

	return distance;
}

BoundingVolume Box::doBoundingVolume() const
{
	Math::Point points[] = { Math::Point(1, 1, 1), Math::Point(1, 1, -1), Math::Point(-1, 1, -1), Math::Point(-1, 1, 1),
							 Math::Point(1, -1, 1), Math::Point(1, -1, -1), Math::Point(-1, -1, -1), Math::Point(-1, -1, 1) };

	BoundingVolume volume;
	for (const Math::Point &point : points) {
		volume.expand(transformation() * point);
	}

	return volume;
}

}
}
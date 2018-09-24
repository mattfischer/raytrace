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

void Box::testIntersect(const Math::Ray &ray, const Math::Normal &normal, Intersection &intersection) const
{
	float scale = (Math::Vector(ray.origin()) * normal - 1) / (ray.direction() * -normal);
	if (scale > 0)
	{
		Math::Point point = ray.origin() + ray.direction() * scale;
		point = point - Math::Vector(normal) * (Math::Vector(point) * normal - 1);

		if(abs(point.x()) <= 1 && abs(point.y()) <= 1 && abs(point.z()) <= 1)
    	{
			if (!intersection.valid() || intersection.distance() > scale) {
				intersection = Intersection(this, ray, scale, normal, point);
			}
		}
	}
}

Intersection Box::doIntersect(const Math::Ray &ray) const
{
	Intersection intersection;

	testIntersect(ray, Math::Normal(1,0,0), intersection);
	testIntersect(ray, Math::Normal(-1,0,0), intersection);
	testIntersect(ray, Math::Normal(0,1,0), intersection);
	testIntersect(ray, Math::Normal(0,-1,0), intersection);
	testIntersect(ray, Math::Normal(0,0,1), intersection);
	testIntersect(ray, Math::Normal(0,0,-1), intersection);

	return intersection;
}

bool Box::doInside(const Math::Point &point) const
{
	return abs(point.x()) <= 1 && abs(point.y()) <= 1 && abs(point.z()) <= 1;
}

BoundingVolume Box::doBoundingVolume(const std::vector<Math::Vector> &vectors) const
{
	std::vector<float> mins;
	std::vector<float> maxes;
	Math::Point points[] = { Math::Point(1, 1, 1), Math::Point(1, 1, -1), Math::Point(-1, 1, -1), Math::Point(-1, 1, 1),
							 Math::Point(1, -1, 1), Math::Point(1, -1, -1), Math::Point(-1, -1, -1), Math::Point(-1, -1, 1) };

	for (const Math::Vector &vector : vectors) {
		float min = FLT_MAX;
		float max = -FLT_MAX;
		for (const Math::Point &point : points) {
			float dist = vector * Math::Vector(point) / (vector.magnitude2());
			min = std::min(min, dist);
			max = std::max(max, dist);
		}
		mins.push_back(min);
		maxes.push_back(max);
	}

	return BoundingVolume(mins, maxes);
}

}
}
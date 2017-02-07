#include "Object/Primitive/Box.hpp"

#include "Object/Primitive/Plane.hpp"

#include <math.h>

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

void Box::testIntersect(const Trace::Ray &ray, const Math::Normal &normal, Trace::IntersectionVector &intersections) const
{
	Trace::Intersection newIntersection = Plane::intersectPlane(this, ray, normal, 1);
	Math::Point point;

	if(newIntersection.valid())
	{
		point = newIntersection.objectPoint();

		if(abs(point.x()) <= 1 && abs(point.y()) <= 1 && abs(point.z()) <= 1)
    	{
			intersections.push_back(newIntersection);
		}
	}
}

void Box::doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const
{
	Trace::Intersection intersection;
	Trace::Intersection newIntersection;
	Math::Point point;

	float t = -Math::Vector(ray.origin()) * ray.direction() / ray.direction().magnitude2();
	Math::Point v = ray.origin() + ray.direction() * t;
	if(Math::Vector(v).magnitude2()>3) return;

	testIntersect(ray, Math::Normal(1,0,0), intersections);
	testIntersect(ray, Math::Normal(-1,0,0), intersections);
	testIntersect(ray, Math::Normal(0,1,0), intersections);
	testIntersect(ray, Math::Normal(0,-1,0), intersections);
	testIntersect(ray, Math::Normal(0,0,1), intersections);
	testIntersect(ray, Math::Normal(0,0,-1), intersections);
}

bool Box::doInside(const Math::Point &point) const
{
	return abs(point.x()) <= 1 && abs(point.y()) <= 1 && abs(point.z()) <= 1;
}

}
}
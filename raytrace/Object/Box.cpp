#include "Object/Box.hpp"

#include "Object/Plane.hpp"

#include <math.h>

namespace Object {

Box::Box()
{
}

Box *Box::fromAst(AST *ast)
{
	return new Box();
}

void Box::testIntersect(const Math::Ray &ray, const Math::Vector &normal, std::vector<Trace::Intersection> &intersections) const
{
	Trace::Intersection newIntersection = Plane::intersectPlane(this, ray, normal, 1);
	Math::Vector point;

	if(newIntersection.valid())
	{
		point = newIntersection.objectPoint();

		if(abs(point.x()) <= 1 && abs(point.y()) <= 1 && abs(point.z()) <= 1)
    	{
			intersections.push_back(newIntersection);
		}
	}
}

void Box::doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const
{
	Trace::Intersection intersection;
	Trace::Intersection newIntersection;
	Math::Vector point;

	double t = -ray.origin() * ray.direction() / ray.direction().magnitude2();
	Math::Vector v = ray.origin() + ray.direction() * t;
	if(v.magnitude2()>3) return;

	testIntersect(ray, Math::Vector(1,0,0), intersections);
	testIntersect(ray, Math::Vector(-1,0,0), intersections);
	testIntersect(ray, Math::Vector(0,1,0), intersections);
	testIntersect(ray, Math::Vector(0,-1,0), intersections);
	testIntersect(ray, Math::Vector(0,0,1), intersections);
	testIntersect(ray, Math::Vector(0,0,-1), intersections);
}

bool Box::doInside(const Math::Vector &point) const
{
	return abs(point.x()) <= 1 && abs(point.y()) <= 1 && abs(point.z()) <= 1;
}

}
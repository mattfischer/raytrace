#include "box.h"

#include "plane.h"

#include <math.h>

Box::Box()
{
}

void Box::testIntersect(const Ray &ray, const Vector &normal, std::vector<Intersection> &intersections) const
{
	Intersection newIntersection = Plane::intersectPlane(this, ray, normal, 1);
	Vector point;

	if(newIntersection.valid())
	{
		point = newIntersection.objectPoint();

		if(abs(point.x()) <= 1 && abs(point.y()) <= 1 && abs(point.z()) <= 1)
    	{
			intersections.push_back(newIntersection);
		}
	}
}

void Box::doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const
{
	Intersection intersection;
	Intersection newIntersection;
	Vector point;

	double t = -ray.origin() * ray.direction() / ray.direction().magnitude2();
	Vector v = ray.origin() + ray.direction() * t;
	if(v.magnitude2()>3) return;

	testIntersect(ray, Vector(1,0,0), intersections);
	testIntersect(ray, Vector(-1,0,0), intersections);
	testIntersect(ray, Vector(0,1,0), intersections);
	testIntersect(ray, Vector(0,-1,0), intersections);
	testIntersect(ray, Vector(0,0,1), intersections);
	testIntersect(ray, Vector(0,0,-1), intersections);
}

bool Box::doInside(const Vector &point) const
{
	return abs(point.x()) <= 1 && abs(point.y()) <= 1 && abs(point.z()) <= 1;
}
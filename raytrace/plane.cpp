#include "plane.h"

#include <math.h>

#define EPSILON .01

Plane::Plane()
{
}

Intersection Plane::intersectPlane(const Primitive *primitive, const Ray &ray, const Vector &normal, double displacement)
{
	Intersection intersection;
	double scale;

	scale = (ray.origin() * normal - displacement) / (ray.direction() * -normal);
	if(scale > EPSILON)
	{
		Vector point = ray.origin() + ray.direction() * scale;
		point = point - normal * (point * normal - displacement);

		intersection = Intersection(primitive, scale, normal, point);
	}

	return intersection;
}

void Plane::doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const
{
	Intersection intersection = intersectPlane(this, ray, Vector(0, 1, 0), 0);
	
	if(intersection.valid())
		intersections.push_back(intersection);
}

bool Plane::doInside(const Vector &point) const
{
	return point.z() <= 0;
}
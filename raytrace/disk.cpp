#include "disk.h"

#include "plane.h"

Disk::Disk()
{
}

Disk::~Disk()
{
}

Intersection Disk::intersectDisk(const Primitive *primitive, const Ray &ray, const Vector &normal, double displacement)
{
	Intersection intersection;
	Intersection planeIntersection = Plane::intersectPlane(primitive, ray, normal, displacement);

	if(planeIntersection.valid())
	{
		Vector point = planeIntersection.objectPoint();

		point = point - normal * (point * normal);

		if(point.magnitude2() <= 1)
			intersection = planeIntersection;
	}

	return intersection;
}

void Disk::doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const
{
	Intersection intersection = intersectDisk(this, ray, Vector(0, 1, 0), 0);
	
	if(intersection.valid())
		intersections.push_back(intersection);
}
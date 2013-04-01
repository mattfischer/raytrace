#include "Object/Disk.hpp"

#include "Object/Plane.hpp"

namespace Object {

Disk::Disk()
{
}

Disk::~Disk()
{
}

Intersection Disk::intersectDisk(const Primitive *primitive, const Math::Ray &ray, const Math::Vector &normal, double displacement)
{
	Intersection intersection;
	Intersection planeIntersection = Plane::intersectPlane(primitive, ray, normal, displacement);

	if(planeIntersection.valid())
	{
		Math::Vector point = planeIntersection.objectPoint();

		point = point - normal * (point * normal);

		if(point.magnitude2() <= 1)
			intersection = planeIntersection;
	}

	return intersection;
}

void Disk::doIntersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const
{
	Intersection intersection = intersectDisk(this, ray, Math::Vector(0, 1, 0), 0);
	
	if(intersection.valid())
		intersections.push_back(intersection);
}

bool Disk::doInside(const Math::Vector &point) const
{
	return false;
}

}
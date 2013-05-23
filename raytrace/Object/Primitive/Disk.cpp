#include "Object/Primitive/Disk.hpp"

#include "Object/Primitive/Plane.hpp"

namespace Object {
namespace Primitive {

Disk::Disk()
{
}

Disk::~Disk()
{
}

Trace::Intersection Disk::intersectDisk(const Base *primitive, const Math::Ray &ray, const Math::Normal &normal, double displacement)
{
	Trace::Intersection intersection;
	Trace::Intersection planeIntersection = Plane::intersectPlane(primitive, ray, normal, displacement);

	if(planeIntersection.valid())
	{
		Math::Point point = planeIntersection.objectPoint();

		point = point - Math::Vector(normal) * (Math::Vector(point) * normal);

		if(Math::Vector(point).magnitude2() <= 1)
			intersection = planeIntersection;
	}

	return intersection;
}

void Disk::doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const
{
	Trace::Intersection intersection = intersectDisk(this, ray, Math::Normal(0, 1, 0), 0);
	
	if(intersection.valid())
		intersections.push_back(intersection);
}

bool Disk::doInside(const Math::Point &point) const
{
	return false;
}

}
}
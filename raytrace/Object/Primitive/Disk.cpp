#include "Object/Primitive/Disk.hpp"

namespace Object {
namespace Primitive {

Trace::Intersection Disk::intersectDisk(const Base *primitive, const Trace::Ray &ray, const Math::Normal &normal, float displacement)
{
	Trace::Intersection intersection;
	Trace::Intersection planeIntersection;
	float scale = (Math::Vector(ray.origin()) * normal - displacement) / (ray.direction() * -normal);
	if (scale > 0)
	{
		Math::Point point = ray.origin() + ray.direction() * scale;
		point = point - Math::Vector(normal) * (Math::Vector(point) * normal - displacement);

		planeIntersection = Trace::Intersection(primitive, ray, scale, normal, point);
	}

	if(planeIntersection.valid())
	{
		Math::Point point = planeIntersection.objectPoint();

		point = point - Math::Vector(normal) * (Math::Vector(point) * normal);

		if(Math::Vector(point).magnitude2() <= 1)
			intersection = planeIntersection;
	}

	return intersection;
}

void Disk::doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
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
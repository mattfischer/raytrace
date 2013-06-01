#include "Object/Primitive/Cylinder.hpp"

#include "Object/Primitive/Disk.hpp"

#include <math.h>

#define EPSILON 0.01

namespace Object {
namespace Primitive {

Cylinder::Cylinder()
{
}

Cylinder::~Cylinder()
{
}

Cylinder *Cylinder::fromAst(AST *ast)
{
	return new Cylinder();
}

void Cylinder::doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
{
	float a, b, c;
	float disc;

	a = ray.direction().x() * ray.direction().x() + ray.direction().y() * ray.direction().y();
	b = 2 * (ray.origin().x() * ray.direction().x() + ray.origin().y() * ray.direction().y());
	c = ray.origin().x() * ray.origin().x() + ray.origin().y() * ray.origin().y() - 1;

	disc = b * b - 4 * a * c;
	if(disc >= 0)
	{
		float distance = (-b - sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			
			if(abs(point.z()) <= 1)
			{
				Math::Vector vector(point);
				vector.setZ(0);
				Math::Normal normal(vector.normalize());
				
				intersections.push_back(Trace::Intersection(this, distance, normal, point));
			}
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			
			if(abs(point.z()) <= 1)
			{
				Math::Vector vector(point);
				vector.setZ(0);
				Math::Normal normal(vector.normalize());
				
				intersections.push_back(Trace::Intersection(this, distance, normal, point));
			}
		}
	}

	Trace::Intersection intersection;

	intersection = Disk::intersectDisk(this, ray, Math::Normal(0, 0, 1), 1);
	if(intersection.valid())
		intersections.push_back(intersection);

	intersection = Disk::intersectDisk(this, ray, Math::Normal(0, 0, -1), 1);
	if(intersection.valid())
		intersections.push_back(intersection);
}

bool Cylinder::doInside(const Math::Point &point) const
{
	return abs(point.z()) <= 1 && point.x() * point.x() + point.y() * point.y() <= 1;
}

}
}
#include "Object/Cylinder.hpp"

#include "Object/Disk.hpp"

#include <math.h>

#define EPSILON 0.01

namespace Object {

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

void Cylinder::doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const
{
	double a, b, c;
	double disc;

	a = ray.direction().x() * ray.direction().x() + ray.direction().y() * ray.direction().y();
	b = 2 * (ray.origin().x() * ray.direction().x() + ray.origin().y() * ray.direction().y());
	c = ray.origin().x() * ray.origin().x() + ray.origin().y() * ray.origin().y() - 1;

	disc = b * b - 4 * a * c;
	if(disc >= 0)
	{
		double distance = (-b - sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			
			if(abs(point.z()) <= 1)
			{
				Math::Vector normal = point.vector();
				normal.setZ(0);
				normal = normal.normalize();
				
				intersections.push_back(Trace::Intersection(this, distance, normal, point));
			}
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			
			if(abs(point.z()) <= 1)
			{
				Math::Vector normal = point.vector();
				normal.setZ(0);
				normal = normal.normalize();
				
				intersections.push_back(Trace::Intersection(this, distance, normal, point));
			}
		}
	}

	Trace::Intersection intersection;

	intersection = Disk::intersectDisk(this, ray, Math::Vector(0, 0, 1), 1);
	if(intersection.valid())
		intersections.push_back(intersection);

	intersection = Disk::intersectDisk(this, ray, Math::Vector(0, 0, -1), 1);
	if(intersection.valid())
		intersections.push_back(intersection);
}

bool Cylinder::doInside(const Math::Point &point) const
{
	return abs(point.z()) <= 1 && point.x() * point.x() + point.y() * point.y() <= 1;
}

}
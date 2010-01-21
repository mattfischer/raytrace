#include "cylinder.h"

#include "disk.h"

#include <math.h>

#define EPSILON 0.01

Cylinder::Cylinder()
{
}

Cylinder::~Cylinder()
{
}

void Cylinder::doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const
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
			Vector point = ray.origin() + ray.direction() * distance;
			
			if(abs(point.z()) <= 1)
			{
				Vector normal = point;
				normal.setZ(0);
				normal = normal.normalize();
				
				intersections.push_back(Intersection(this, distance, normal, point));
			}
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Vector point = ray.origin() + ray.direction() * distance;
			
			if(abs(point.z()) <= 1)
			{
				Vector normal = point;
				normal.setZ(0);
				normal = normal.normalize();
				
				intersections.push_back(Intersection(this, distance, normal, point));
			}
		}
	}

	Intersection intersection;

	intersection = Disk::intersectDisk(this, ray, Vector(0, 0, 1), 1);
	if(intersection.valid())
		intersections.push_back(intersection);

	intersection = Disk::intersectDisk(this, ray, Vector(0, 0, -1), 1);
	if(intersection.valid())
		intersections.push_back(intersection);
}

bool Cylinder::doInside(const Vector &point) const
{
	return abs(point.z()) <= 1 && point.x() * point.x() + point.y() * point.y() <= 1;
}
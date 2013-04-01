#include "cone.h"

#include "disk.h"

#include <math.h>

#define EPSILON 0.01
Cone::Cone()
{
}

Cone::~Cone()
{
}

Cone *Cone::fromAst(AST *ast)
{
	return new Cone();
}

void Cone::doIntersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const
{
	double a, b, c;
	double disc;

	a = ray.direction().x() * ray.direction().x() + ray.direction().y() * ray.direction().y() - ray.direction().z() * ray.direction().z();
	b = 2 * (ray.origin().x() * ray.direction().x() + ray.origin().y() * ray.direction().y() - ray.origin().z() * ray.direction().z());
	c = ray.origin().x() * ray.origin().x() + ray.origin().y() * ray.origin().y() - ray.origin().z() * ray.origin().z();

	disc = b * b - 4 * a * c;
	if(disc >= 0)
	{
		double distance = (-b - sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Vector point = ray.origin() + ray.direction() * distance;
			
			if(point.z() >= 0 && point.z() <= 1)
			{
				Math::Vector normal(point);
				normal.setZ(0);
				normal = normal.normalize();
				normal.setZ(-1);
				normal = normal.normalize();

				intersections.push_back(Intersection(this, distance, normal, point));
			}
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Vector point = ray.origin() + ray.direction() * distance;
			
			if(point.z() >= 0 && point.z() <= 1)
			{
				Math::Vector normal(point);
				normal.setZ(0);
				normal = normal.normalize();
				normal.setZ(-1);
				normal = normal.normalize();

				intersections.push_back(Intersection(this, distance, normal, point));
			}
		}
	}

	Intersection intersection = Disk::intersectDisk(this, ray, Math::Vector(0, 0, 1), 1);
	if(intersection.valid())
		intersections.push_back(intersection);
}

bool Cone::doInside(const Math::Vector &point) const
{
	return point.z() <= 1 && point.x() * point.x() + point.y() * point.y() <= point.z() * point.z();
}
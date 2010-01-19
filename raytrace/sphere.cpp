#include "sphere.h"
#include "transformation.h"

#include <math.h>

#define EPSILON 0.0001

Sphere::Sphere()
{
}

void Sphere::doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const
{
	Intersection intersection;
	double a, b, c;
	double disc;

	a = ray.direction().magnitude2();
	b = 2 * (ray.origin() * ray.direction());
	c = ray.origin().magnitude2() - 1;

	disc = b * b - 4 * a * c;
	if(disc >= 0)
	{
		double distance = (-b - sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Vector point = ray.origin() + ray.direction() * distance;
			intersections.push_back(Intersection(this, distance, point, point));
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Vector point = ray.origin() + ray.direction() * distance;
			intersections.push_back(Intersection(this, distance, point, point));
		}
	}
}


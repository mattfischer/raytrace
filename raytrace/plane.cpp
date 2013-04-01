#include "plane.h"

#include <math.h>

#define EPSILON .01

Plane::Plane()
{
}

Plane *Plane::fromAst(AST *ast)
{
	return new Plane;
}

Intersection Plane::intersectPlane(const Primitive *primitive, const Math::Ray &ray, const Math::Vector &normal, double displacement)
{
	double scale;

	scale = (ray.origin() * normal - displacement) / (ray.direction() * -normal);
	if(scale > EPSILON)
	{
		Math::Vector point = ray.origin() + ray.direction() * scale;
		point = point - normal * (point * normal - displacement);

		return Intersection(primitive, scale, normal, point);
	}

	return Intersection();
}

void Plane::doIntersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const
{
	double scale;

	if(ray.origin().y() > 0 && ray.direction().y() > 0 || 
		ray.origin().y() < 0 && ray.direction().y() < 0) return;

	scale = -ray.origin().y() / ray.direction().y();
	if(scale > EPSILON)
	{
		Math::Vector point = ray.origin() + ray.direction() * scale;

		intersections.push_back(Intersection(this, scale, Math::Vector(0,1,0), point));
	}
}

bool Plane::doInside(const Math::Vector &point) const
{
	return point.y() <= 0;
}
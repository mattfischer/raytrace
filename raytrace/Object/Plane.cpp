#include "Object/Plane.hpp"

#include <math.h>

#define EPSILON .01

namespace Object {

Plane::Plane()
{
}

Plane *Plane::fromAst(AST *ast)
{
	return new Plane;
}

Trace::Intersection Plane::intersectPlane(const Primitive *primitive, const Math::Ray &ray, const Math::Vector &normal, double displacement)
{
	double scale;

	scale = (ray.origin().vector() * normal - displacement) / (ray.direction() * -normal);
	if(scale > EPSILON)
	{
		Math::Point point = ray.origin() + ray.direction() * scale;
		point = point - normal * (point.vector() * normal - displacement);

		return Trace::Intersection(primitive, scale, normal, point);
	}

	return Trace::Intersection();
}

void Plane::doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const
{
	double scale;

	if(ray.origin().y() > 0 && ray.direction().y() > 0 || 
		ray.origin().y() < 0 && ray.direction().y() < 0) return;

	scale = -ray.origin().y() / ray.direction().y();
	if(scale > EPSILON)
	{
		Math::Point point = ray.origin() + ray.direction() * scale;

		intersections.push_back(Trace::Intersection(this, scale, Math::Vector(0,1,0), point));
	}
}

bool Plane::doInside(const Math::Point &point) const
{
	return point.y() <= 0;
}

}
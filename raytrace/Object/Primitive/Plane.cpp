#include "Object/Primitive/Plane.hpp"

#include <math.h>

#define EPSILON .01

namespace Object {
namespace Primitive {

Plane::Plane()
{
}

Plane *Plane::fromAst(AST *ast)
{
	return new Plane;
}

Trace::Intersection Plane::intersectPlane(const Base *primitive, const Math::Ray &ray, const Math::Normal &normal, double displacement)
{
	double scale;

	scale = (Math::Vector(ray.origin()) * normal - displacement) / (ray.direction() * -normal);
	if(scale > EPSILON)
	{
		Math::Point point = ray.origin() + ray.direction() * scale;
		point = point - Math::Vector(normal) * (Math::Vector(point) * normal - displacement);

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

		intersections.push_back(Trace::Intersection(this, scale, Math::Normal(0,1,0), point));
	}
}

bool Plane::doInside(const Math::Point &point) const
{
	return point.y() <= 0;
}

}
}
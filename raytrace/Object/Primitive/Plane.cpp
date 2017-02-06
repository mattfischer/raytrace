#include "Object/Primitive/Plane.hpp"

#include <math.h>

#define EPSILON .01

namespace Object {
namespace Primitive {

std::unique_ptr<Plane> Plane::fromAst(AST *ast)
{
	return std::make_unique<Plane>();
}

Trace::Intersection Plane::intersectPlane(const Base *primitive, const Trace::Ray &ray, const Math::Normal &normal, float displacement)
{
	float scale;

	scale = (Math::Vector(ray.origin()) * normal - displacement) / (ray.direction() * -normal);
	if(scale > EPSILON)
	{
		Math::Point point = ray.origin() + ray.direction() * scale;
		point = point - Math::Vector(normal) * (Math::Vector(point) * normal - displacement);

		return Trace::Intersection(primitive, ray, scale, normal, point);
	}

	return Trace::Intersection();
}

void Plane::doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
{
	float scale;

	if(ray.origin().y() > 0 && ray.direction().y() > 0 || 
		ray.origin().y() < 0 && ray.direction().y() < 0) return;

	scale = -ray.origin().y() / ray.direction().y();
	if(scale > EPSILON)
	{
		Math::Point point = ray.origin() + ray.direction() * scale;

		intersections.push_back(Trace::Intersection(this, ray, scale, Math::Normal(0,1,0), point));
	}
}

bool Plane::doInside(const Math::Point &point) const
{
	return point.y() <= 0;
}

}
}
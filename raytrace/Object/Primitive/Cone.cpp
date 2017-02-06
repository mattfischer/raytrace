#include "Object/Primitive/Cone.hpp"

#include "Object/Primitive/Disk.hpp"

#include <math.h>

#define EPSILON 0.01

namespace Object {
namespace Primitive {

Cone::Cone()
{
}

Cone::~Cone()
{
}

std::unique_ptr<Cone> Cone::fromAst(AST *ast)
{
	return std::make_unique<Cone>();
}

void Cone::doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
{
	float a, b, c;
	float disc;

	a = ray.direction().x() * ray.direction().x() + ray.direction().y() * ray.direction().y() - ray.direction().z() * ray.direction().z();
	b = 2 * (ray.origin().x() * ray.direction().x() + ray.origin().y() * ray.direction().y() - ray.origin().z() * ray.direction().z());
	c = ray.origin().x() * ray.origin().x() + ray.origin().y() * ray.origin().y() - ray.origin().z() * ray.origin().z();

	disc = b * b - 4 * a * c;
	if(disc >= 0)
	{
		float distance = (-b - sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			
			if(point.z() >= 0 && point.z() <= 1)
			{
				Math::Vector vector(point);
				vector.setZ(0);
				vector = vector.normalize();
				vector.setZ(-1);
				Math::Normal normal(vector.normalize());

				intersections.push_back(Trace::Intersection(this, ray, distance, normal, point));
			}
		}

		distance = (-b + sqrt(disc)) / (2 * a);

		if(distance > EPSILON)
		{
			Math::Point point = ray.origin() + ray.direction() * distance;
			
			if(point.z() >= 0 && point.z() <= 1)
			{
				Math::Vector vector(point);
				vector.setZ(0);
				vector = vector.normalize();
				vector.setZ(-1);
				Math::Normal normal(vector.normalize());

				intersections.push_back(Trace::Intersection(this, ray, distance, normal, point));
			}
		}
	}

	Trace::Intersection intersection = Disk::intersectDisk(this, ray, Math::Normal(0, 0, 1), 1);
	if(intersection.valid())
		intersections.push_back(intersection);
}

bool Cone::doInside(const Math::Point &point) const
{
	return point.z() <= 1 && point.x() * point.x() + point.y() * point.y() <= point.z() * point.z();
}

}
}
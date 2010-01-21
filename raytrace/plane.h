#ifndef PLANE_H
#define PLANE_H

#include "primitive.h"

class Plane : public Primitive
{
public:
	Plane();

	static Intersection intersectPlane(const Primitive *primitive, const Ray &ray, const Vector &normal, double displacement);

protected:
	virtual void doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Vector &point) const;
};

#endif

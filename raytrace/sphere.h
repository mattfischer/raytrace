#ifndef SPHERE_H
#define SPHERE_H

#include "primitive.h"

class Sphere : public Primitive
{
public:
	Sphere();

protected:
	virtual void doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Vector &point) const;
};

#endif

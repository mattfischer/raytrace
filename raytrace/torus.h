#ifndef TORUS_H
#define TORUS_H

#include "primitive.h"

class Torus : public Primitive
{
public:
	Torus();

protected:
	virtual void doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Vector &point) const;
};

#endif
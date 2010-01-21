#ifndef CYLINDER_H
#define CYLINDER_H

#include "primitive.h"

class Cylinder : public Primitive
{
public:
	Cylinder();
	~Cylinder();

protected:
	virtual void doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Vector &point) const;
};

#endif
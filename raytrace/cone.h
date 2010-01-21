#ifndef CONE_H
#define CONE_H

#include "primitive.h"

class Cone : public Primitive
{
public:
	Cone();
	~Cone();

protected:
	virtual void doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Vector &point) const;
};

#endif
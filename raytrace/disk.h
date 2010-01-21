#ifndef DISK_H
#define DISK_H

#include "primitive.h"

class Disk : public Primitive
{
public:
	Disk();
	virtual ~Disk();

	static Intersection intersectDisk(const Primitive *primitive, const Ray &ray, const Vector &normal, double displacement);

protected:
	virtual void doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Vector &point) const;
};

#endif
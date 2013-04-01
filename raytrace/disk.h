#ifndef DISK_H
#define DISK_H

#include "primitive.h"

class Disk : public Primitive
{
public:
	Disk();
	virtual ~Disk();

	static Intersection intersectDisk(const Primitive *primitive, const Math::Ray &ray, const Math::Vector &normal, double displacement);

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Math::Vector &point) const;
};

#endif
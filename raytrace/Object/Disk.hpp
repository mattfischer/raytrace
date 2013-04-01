#ifndef DISK_H
#define DISK_H

#include "Object/Primitive.hpp"

namespace Object {

class Disk : public Primitive
{
public:
	Disk();
	virtual ~Disk();

	static Trace::Intersection intersectDisk(const Primitive *primitive, const Math::Ray &ray, const Math::Vector &normal, double displacement);

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
#endif
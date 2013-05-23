#ifndef DISK_H
#define DISK_H

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Disk : public Base
{
public:
	Disk();
	virtual ~Disk();

	static Trace::Intersection intersectDisk(const Base *primitive, const Trace::Ray &ray, const Math::Normal &normal, float displacement);

protected:
	virtual void doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif
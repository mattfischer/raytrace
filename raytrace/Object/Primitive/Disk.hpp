#ifndef OBJECT_PRIMITIVE_DISK_HPP
#define OBJECT_PRIMITIVE_DISK_HPP

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Disk : public Base
{
public:
	static Trace::Intersection intersectDisk(const Base *primitive, const Trace::Ray &ray, const Math::Normal &normal, float displacement);

protected:
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif
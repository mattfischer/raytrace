#ifndef OBJECT_PRIMITIVE_TORUS_HPP
#define OBJECT_PRIMITIVE_TORUS_HPP

#include "Parse/Forwards.hpp"

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Torus : public Base
{
public:

protected:
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif
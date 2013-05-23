#ifndef TORUS_H
#define TORUS_H

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Torus : public Base
{
public:
	Torus();

protected:
	virtual void doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif
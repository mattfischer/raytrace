#ifndef TORUS_H
#define TORUS_H

#include "Object/Primitive.hpp"

namespace Object {

class Torus : public Primitive
{
public:
	Torus();

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Vector &point) const;
};

}
#endif
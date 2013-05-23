#ifndef SPHERE_H
#define SPHERE_H

#include "Object/Primitive/Base.hpp"
#include "Parse/AST.h"

namespace Object {
namespace Primitive {

class Sphere : public Base
{
public:
	Sphere();
	static Sphere *fromAst(AST *ast);

protected:
	virtual void doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif

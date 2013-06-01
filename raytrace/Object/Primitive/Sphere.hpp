#ifndef OBJECT_PRIMITIVE_SPHERE_HPP
#define OBJECT_PRIMITIVE_SPHERE_HPP

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
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif

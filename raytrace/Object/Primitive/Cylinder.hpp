#ifndef OBJECT_PRIMITIVE_CYLINDER_HPP
#define OBJECT_PRIMITIVE_CYLINDER_HPP

#include "Object/Primitive/Base.hpp"
#include "Parse/AST.h"

namespace Object {
namespace Primitive {

class Cylinder : public Base
{
public:
	Cylinder();
	~Cylinder();

	static Cylinder *fromAst(AST *ast);

protected:
	virtual void doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif
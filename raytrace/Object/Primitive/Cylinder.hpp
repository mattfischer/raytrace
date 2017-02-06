#ifndef OBJECT_PRIMITIVE_CYLINDER_HPP
#define OBJECT_PRIMITIVE_CYLINDER_HPP

#include "Parse/Forwards.hpp"

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Cylinder : public Base
{
public:
	static std::unique_ptr<Cylinder> fromAst(AST *ast);

protected:
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif
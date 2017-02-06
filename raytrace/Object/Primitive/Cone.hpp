#ifndef OBJECT_PRIMITIVE_CONE_HPP
#define OBJECT_PRIMITIVE_CONE_HPP

#include "Parse/Forwards.hpp"
#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Cone : public Base
{
public:
	static std::unique_ptr<Cone> fromAst(AST *ast);

protected:
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif
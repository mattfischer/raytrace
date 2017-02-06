#ifndef OBJECT_PRIMITIVE_PLANE_HPP
#define OBJECT_PRIMITIVE_PLANE_HPP

#include "Parse/Forwards.hpp"

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Plane : public Base
{
public:
	Plane();

	static std::unique_ptr<Plane> fromAst(AST *ast);

	static Trace::Intersection intersectPlane(const Base *primitive, const Trace::Ray &ray, const Math::Normal &normal, float displacement);

protected:
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif

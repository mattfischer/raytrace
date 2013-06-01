#ifndef OBJECT_PRIMITIVE_PLANE_HPP
#define OBJECT_PRIMITIVE_PLANE_HPP

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Plane : public Base
{
public:
	Plane();

	static Plane *fromAst(AST *ast);

	static Trace::Intersection intersectPlane(const Base *primitive, const Trace::Ray &ray, const Math::Normal &normal, float displacement);

protected:
	virtual void doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif

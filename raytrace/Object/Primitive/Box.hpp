#ifndef OBJECT_PRIMITIVE_BOX_HPP
#define OBJECT_PRIMITIVE_BOX_HPP

#include "Parse/Forwards.hpp"

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Box : public Base
{
public:
	static std::unique_ptr<Box> fromAst(AST *ast);

protected:
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
	virtual BoundingSphere doBoundingSphere() const;

	void testIntersect(const Trace::Ray &ray, const Math::Normal &normal, std::vector<Trace::Intersection> &intersections) const;
};

}
}
#endif

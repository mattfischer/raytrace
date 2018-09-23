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
	virtual Trace::Intersection doIntersect(const Trace::Ray &ray) const;
	virtual bool doInside(const Math::Point &point) const;
	virtual BoundingVolume doBoundingVolume(const std::vector<Math::Vector> &vectors) const;

	void testIntersect(const Trace::Ray &ray, const Math::Normal &normal, Trace::Intersection &intersection) const;
};

}
}
#endif

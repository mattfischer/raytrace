#ifndef OBJECT_PRIMITIVE_BOX_HPP
#define OBJECT_PRIMITIVE_BOX_HPP

#include "Parse/AST.h"

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Box : public Base
{
public:
	static std::unique_ptr<Box> fromAst(AST *ast);

protected:
	virtual Intersection doIntersect(const Math::Ray &ray) const;
	virtual bool doInside(const Math::Point &point) const;
	virtual BoundingVolume doBoundingVolume() const;

	void testIntersect(const Math::Ray &ray, const Math::Normal &normal, Intersection &intersection) const;
};

}
}
#endif

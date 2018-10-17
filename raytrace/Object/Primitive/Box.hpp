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
	virtual float doIntersect(const Math::Ray &ray, Math::Normal &normal) const;
	virtual BoundingVolume doBoundingVolume() const;

	void testIntersect(const Math::Ray &ray, const Math::Normal &normal, float &outDistance, Math::Normal &outNormal) const;
};

}
}
#endif

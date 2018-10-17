#ifndef OBJECT_PRIMITIVE_SPHERE_HPP
#define OBJECT_PRIMITIVE_SPHERE_HPP

#include "Parse/AST.h"

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Sphere : public Base
{
public:
	static std::unique_ptr<Sphere> fromAst(AST *ast);

	Sphere(const Math::Point &position, float radius);

protected:
	virtual float doIntersect(const Math::Ray &ray, Math::Normal &normal) const;
	virtual BoundingVolume doBoundingVolume() const;

private:
	Math::Point mPosition;
	float mRadius;
};

}
}
#endif

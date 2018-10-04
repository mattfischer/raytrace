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
	virtual Intersection doIntersect(const Math::Ray &ray) const;
	virtual bool doInside(const Math::Point &point) const;
	virtual BoundingVolume doBoundingVolume() const;

private:
	Math::Point mPosition;
	float mRadius;
};

}
}
#endif

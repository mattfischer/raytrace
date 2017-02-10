#ifndef OBJECT_PRIMITIVE_SPHERE_HPP
#define OBJECT_PRIMITIVE_SPHERE_HPP

#include "Parse/Forwards.hpp"

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Sphere : public Base
{
public:
	static std::unique_ptr<Sphere> fromAst(AST *ast);

protected:
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
	virtual BoundingSphere doBoundingSphere() const;
};

}
}
#endif

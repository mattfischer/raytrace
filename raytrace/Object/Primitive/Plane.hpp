#ifndef PLANE_H
#define PLANE_H

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Plane : public Base
{
public:
	Plane();

	static Plane *fromAst(AST *ast);

	static Trace::Intersection intersectPlane(const Base *primitive, const Math::Ray &ray, const Math::Vector &normal, double displacement);

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif

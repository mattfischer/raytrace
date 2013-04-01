#ifndef SPHERE_H
#define SPHERE_H

#include "Object/Primitive.hpp"
#include "Parse/AST.h"

namespace Object {

class Sphere : public Primitive
{
public:
	Sphere();
	static Sphere *fromAst(AST *ast);

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
#endif

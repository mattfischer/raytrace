#ifndef PLANE_H
#define PLANE_H

#include "primitive.h"

class Plane : public Primitive
{
public:
	Plane();

	static Plane *fromAst(AST *ast);

	static Intersection intersectPlane(const Primitive *primitive, const Math::Ray &ray, const Math::Vector &normal, double displacement);

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Math::Vector &point) const;
};

#endif

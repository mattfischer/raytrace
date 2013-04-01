#ifndef SPHERE_H
#define SPHERE_H

#include "primitive.h"
#include "ast.h"

class Sphere : public Primitive
{
public:
	Sphere();
	static Sphere *fromAst(AST *ast);

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Math::Vector &point) const;
};

#endif

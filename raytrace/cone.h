#ifndef CONE_H
#define CONE_H

#include "primitive.h"
#include "ast.h"

class Cone : public Primitive
{
public:
	Cone();
	~Cone();

	static Cone *fromAst(AST *ast);

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Math::Vector &point) const;
};

#endif
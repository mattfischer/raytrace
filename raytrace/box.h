#ifndef BOX_H
#define BOX_H

#include "primitive.h"
#include "ast.h"

class Box : public Primitive
{
public:
	Box();

	static Box *fromAst(AST *ast);

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const;
	virtual bool doInside(const Math::Vector &point) const;

	void testIntersect(const Math::Ray &ray, const Math::Vector &normal, std::vector<Intersection> &intersections) const;
};

#endif

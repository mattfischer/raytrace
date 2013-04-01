#ifndef CONE_H
#define CONE_H

#include "Object/Primitive.hpp"
#include "Parse/AST.h"

namespace Object {

class Cone : public Primitive
{
public:
	Cone();
	~Cone();

	static Cone *fromAst(AST *ast);

protected:
	virtual void doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
#endif
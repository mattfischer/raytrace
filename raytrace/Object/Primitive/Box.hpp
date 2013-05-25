#ifndef BOX_H
#define BOX_H

#include "Object/Primitive/Base.hpp"
#include "Parse/AST.h"

namespace Object {
namespace Primitive {

class Box : public Base
{
public:
	Box();

	static Box *fromAst(AST *ast);

protected:
	virtual void doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;

	void testIntersect(const Trace::Ray &ray, const Math::Normal &normal, std::vector<Trace::Intersection> &intersections) const;
};

}
}
#endif
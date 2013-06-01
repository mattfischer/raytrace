#ifndef OBJECT_PRIMITIVE_CONE_HPP
#define OBJECT_PRIMITIVE_CONE_HPP

#include "Object/Primitive/Base.hpp"
#include "Parse/AST.h"

namespace Object {
namespace Primitive {

class Cone : public Base
{
public:
	Cone();
	~Cone();

	static Cone *fromAst(AST *ast);

protected:
	virtual void doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	virtual bool doInside(const Math::Point &point) const;
};

}
}
#endif
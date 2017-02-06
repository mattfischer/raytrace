#ifndef OBJECT_PRIMITIVE_CSG_HPP
#define OBJECT_PRIMITIVE_CSG_HPP

#include "Parse/Forwards.hpp"

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Csg : public Base
{
public:
	enum Type
	{
		TypeUnion,
		TypeDifference,
		TypeIntersection
	};

	Csg(Type type, std::unique_ptr<Primitive::Base> &&primitive1, std::unique_ptr<Primitive::Base> &&primitive2);

	static std::unique_ptr<Csg> fromAst(AST *ast);

	Type type() const;
	const Primitive::Base &primitive1() const;
	const Primitive::Base &primitive2() const;


protected:
	Type mType;
	std::unique_ptr<Primitive::Base> mPrimitive1;
	std::unique_ptr<Primitive::Base> mPrimitive2;


	virtual bool doInside(const Math::Point &point) const;
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
};

}
}
#endif
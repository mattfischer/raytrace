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

	static std::unique_ptr<Csg> fromAst(AST *ast);

	const Primitive::Base &primitive1() const;
	void setPrimitive1(std::unique_ptr<Primitive::Base> &&primitive1);

	const Primitive::Base &primitive2() const;
	void setPrimitive2(std::unique_ptr<Primitive::Base> &&primitive2);

	Type type() const;
	void setType(Type type);

protected:
	std::unique_ptr<Primitive::Base> mPrimitive1;
	std::unique_ptr<Primitive::Base> mPrimitive2;
	Type mType;

	virtual bool doInside(const Math::Point &point) const;
	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
};

}
}
#endif
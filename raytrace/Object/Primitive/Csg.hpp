#ifndef CSG_H
#define CSG_H

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

	Csg();

	static Csg *fromAst(AST *ast);

	Primitive::Base *primitive1();
	void setPrimitive1(Primitive::Base *primitive1);

	Primitive::Base *primitive2();
	void setPrimitive2(Primitive::Base *primitive2);

	Type type() const;
	void setType(Type type);

protected:
	Primitive::Base *mPrimitive1;
	Primitive::Base *mPrimitive2;
	Type mType;

	virtual bool doInside(const Math::Point &point) const;
	virtual void doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
};

}
}
#endif
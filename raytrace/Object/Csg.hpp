#ifndef CSG_H
#define CSG_H

#include "Object/Primitive.hpp"

namespace Object {

class Csg : public Primitive
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

	Primitive *primitive1();
	void setPrimitive1(Primitive *primitive1);

	Primitive *primitive2();
	void setPrimitive2(Primitive *primitive2);

	Type type() const;
	void setType(Type type);

protected:
	Primitive *mPrimitive1;
	Primitive *mPrimitive2;
	Type mType;

	virtual bool doInside(const Math::Vector &point) const;
	virtual void doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
};

}
#endif
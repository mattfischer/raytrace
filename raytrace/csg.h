#ifndef CSG_H
#define CSG_H

#include "primitive.h"

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

	virtual bool doInside(const Vector &point) const;
	virtual void doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const;
};

#endif
#include "csg.h"

Csg::Csg()
{
}

Primitive *Csg::primitive1()
{
	return mPrimitive1;
}

void Csg::setPrimitive1(Primitive *primitive1)
{
	mPrimitive1 = primitive1;
}

Primitive *Csg::primitive2()
{
	return mPrimitive2;
}

void Csg::setPrimitive2(Primitive *primitive2)
{
	mPrimitive2 = primitive2;
}

Csg::Type Csg::type() const
{
	return mType;
}

void Csg::setType(Type type)
{
	mType = type;
}
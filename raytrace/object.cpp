#include "object.h"

Object::Object() 
{ 
}

Object::~Object()
{
}

void Object::transform(const Transformation &transformation)
{
	mTransformation = transformation.transformTransformation(mTransformation);
}

const Transformation &Object::transformation() const
{
	return mTransformation;
}


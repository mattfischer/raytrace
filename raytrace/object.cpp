#include "object.h"

Object::Object() 
{ 
}

Object::~Object()
{
}

void Object::transform(const Math::Transformation &transformation)
{
	mTransformation = transformation.transformTransformation(mTransformation);
}

const Math::Transformation &Object::transformation() const
{
	return mTransformation;
}


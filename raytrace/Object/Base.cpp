#include "Object/Base.hpp"

namespace Object {

Base::Base() 
{ 
}

Base::~Base()
{
}

void Base::transform(const Math::Transformation &transformation)
{
	mTransformation = transformation.transformTransformation(mTransformation);
}

const Math::Transformation &Base::transformation() const
{
	return mTransformation;
}

}
#include "Object/Base.hpp"

namespace Object {

void Base::transform(const Math::Transformation &transformation)
{
	mTransformation = transformation * mTransformation;
}

const Math::Transformation &Base::transformation() const
{
	return mTransformation;
}

}
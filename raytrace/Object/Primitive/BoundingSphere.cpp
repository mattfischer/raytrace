#include "Object/Primitive/BoundingSphere.hpp"

namespace Object {
namespace Primitive {

BoundingSphere::BoundingSphere(const Math::Point &origin, float radius)
	: mOrigin(origin), mRadius(radius)
{
}

const Math::Point &BoundingSphere::origin() const
{
	return mOrigin;
}

float BoundingSphere::radius() const
{
	return mRadius;
}

}
}
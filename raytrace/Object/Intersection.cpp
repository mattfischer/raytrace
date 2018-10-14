#include "Object/Intersection.hpp"

#include "Object/Primitive/Base.hpp"

namespace Object {

Intersection::Intersection()
	: mPrimitive(*(Object::Primitive::Base*)0), mRay(*(Math::Ray*)0), mDistance(FLT_MAX)
{
}

Intersection::Intersection(const Object::Primitive::Base &primitive, const Math::Ray &ray, float distance, const Math::Normal &normal)
	: mPrimitive(primitive), mRay(ray), mNormal(normal), mDistance(distance)
{
};

const Math::Normal &Intersection::normal() const
{
	return mNormal;
}

Math::Point Intersection::point() const
{
	return mRay.origin() + mRay.direction() * mDistance;
}

bool Intersection::valid() const
{
	return mDistance != FLT_MAX;
}

const Object::Primitive::Base &Intersection::primitive() const
{
	return mPrimitive;
}

const Math::Ray &Intersection::ray() const
{
	return mRay;
}

float Intersection::distance() const
{
	return mDistance;
}

}
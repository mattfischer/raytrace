#include "Object/Intersection.hpp"

namespace Object {

Intersection::Intersection()
	: mPrimitive(*(Object::Primitive*)0), mRay(*(Math::Ray*)0), mDistance(FLT_MAX)
{
}

Intersection::Intersection(const Object::Primitive &primitive, const Math::Ray &ray, const Math::Point &point, float distance, const Math::Normal &normal, const Object::Color &albedo)
	: mPrimitive(primitive), mRay(ray), mPoint(point), mNormal(normal), mDistance(distance), mAlbedo(albedo)
{
};

const Math::Normal &Intersection::normal() const
{
	return mNormal;
}

const Math::Point &Intersection::point() const
{
	return mPoint;
}

bool Intersection::valid() const
{
	return mDistance != FLT_MAX;
}

const Object::Primitive &Intersection::primitive() const
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

const Object::Color &Intersection::albedo() const
{
	return mAlbedo;
}

}
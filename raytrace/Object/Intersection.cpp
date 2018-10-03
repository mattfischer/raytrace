#include "Object/Intersection.hpp"

#include "Object/Primitive/Base.hpp"

namespace Object {

Intersection::Intersection()
{
	mValid = false;
}

Intersection::Intersection(const Object::Primitive::Base *primitive, const Math::Ray &ray, float distance, const Math::Normal &objectNormal, const Math::Point &objectPoint)
{
	mValid = true;
	mPrimitive = primitive;
	mRay = ray;
	mDistance = distance;
	mObjectNormal = objectNormal;
	mObjectPoint = objectPoint;
	mTransformed = false;
};

Intersection::Intersection(const Intersection &c)
{
	mValid = c.mValid;
	mPrimitive = c.mPrimitive;
	mRay = c.mRay;
	mDistance = c.mDistance;
	mNormal = c.mNormal;
	mObjectNormal = c.mObjectNormal;
	mObjectPoint = c.mObjectPoint;
	mPoint = c.mPoint;
	mTransformed = c.mTransformed;
}

Intersection &Intersection::operator=(const Intersection &c)
{
	mValid = c.mValid;
	mPrimitive = c.mPrimitive;
	mRay = c.mRay;
	mDistance = c.mDistance;
	mNormal = c.mNormal;
	mObjectNormal = c.mObjectNormal;
	mObjectPoint = c.mObjectPoint;
	mPoint = c.mPoint;
	mTransformed = c.mTransformed;

	return *this;
}

const Math::Normal &Intersection::normal() const
{
	if(!mTransformed)
		doTransform();

	return mNormal;
}

const Math::Point &Intersection::point() const
{
	if(!mTransformed)
		doTransform();

	return mPoint;
}

bool Intersection::valid() const
{
	return mValid;
}

const Object::Primitive::Base *Intersection::primitive() const
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

const Math::Normal &Intersection::objectNormal() const
{
	return mObjectNormal;
}

const Math::Point &Intersection::objectPoint() const
{
	return mObjectPoint;
}

const Intersection &Intersection::nearest(const Intersection &b) const
{
	return (*this < b) ? *this : b;
}

bool Intersection::operator<(const Intersection &b) const
{
	if(!valid()) return false;
	if(!b.valid()) return true;

	return distance() < b.distance();
}

void Intersection::doTransform() const
{
	mNormal = mPrimitive->transformation() * mObjectNormal;
	mPoint = mPrimitive->transformation() * mObjectPoint;

	mNormal = mNormal.normalize();

	mTransformed = true;
}

}
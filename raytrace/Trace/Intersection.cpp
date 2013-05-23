#include "Trace/Intersection.hpp"

#include "Object/Primitive/Base.hpp"

namespace Trace {

Intersection::Intersection()
{
	mValid = false;
}

Intersection::Intersection(const Object::Primitive::Base *primitive, float distance, const Math::Normal &objectNormal, const Math::Point &objectPoint)
{
	mValid = true;
	mPrimitive = primitive;
	mDistance = distance;
	mObjectNormal = objectNormal;
	mObjectPoint = objectPoint;
	mTransformed = false;
	mCompositeTransformed = false;
};

Intersection::Intersection(const Intersection &c)
{
	mValid = c.mValid;
	mPrimitive = c.mPrimitive;
	mDistance = c.mDistance;
	mNormal = c.mNormal;
	mObjectNormal = c.mObjectNormal;
	mObjectPoint = c.mObjectPoint;
	mPoint = c.mPoint;
	mTransformed = c.mTransformed;
	mCompositeTransformed = c.mCompositeTransformed;

	if(mCompositeTransformed)
	{
		mTransformation = c.mTransformation;
	}
}

Intersection &Intersection::operator=(const Intersection &c)
{
	mValid = c.mValid;
	mPrimitive = c.mPrimitive;
	mDistance = c.mDistance;
	mNormal = c.mNormal;
	mObjectNormal = c.mObjectNormal;
	mObjectPoint = c.mObjectPoint;
	mPoint = c.mPoint;
	mTransformed = c.mTransformed;
	mCompositeTransformed = c.mCompositeTransformed;

	if(mCompositeTransformed)
	{
		mTransformation = c.mTransformation;
	}

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

void Intersection::transform(const Math::Transformation &transformation)
{
	if(mCompositeTransformed)
	{
		mTransformation = transformation * mTransformation;
	}
	else
	{
		mTransformation = transformation * mPrimitive->transformation();
		mCompositeTransformed = true;
	}

	if(mTransformed)
	{
		mNormal = transformation * mNormal;
		mPoint = transformation * mPoint;
	}
}

const Math::Transformation &Intersection::transformation() const
{
	if(mCompositeTransformed)
	{
		return mTransformation;
	}
	else
	{
		return mPrimitive->transformation();
	}
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
	if(mCompositeTransformed)
	{
		mNormal = mTransformation * mObjectNormal;
		mPoint = mTransformation * mObjectPoint;
	}
	else
	{
		mNormal = mPrimitive->transformation() * mObjectNormal;
		mPoint = mPrimitive->transformation() * mObjectPoint;
	}

	mTransformed = true;
}

}
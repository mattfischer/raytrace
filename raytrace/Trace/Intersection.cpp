#include "Trace/Intersection.hpp"

#include "Object/Primitive/Base.hpp"

namespace Trace {

Intersection::Intersection()
{
	mValid = false;
}

Intersection::Intersection(const Object::Primitive::Base *primitive, double distance, const Math::Vector &objectNormal, const Math::Point &objectPoint)
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

bool Intersection::valid() const
{
	return mValid;
}

void Intersection::setValid(bool valid)
{
	mValid = valid;
}

const Object::Primitive::Base *Intersection::primitive() const
{
	return mPrimitive;
}

void Intersection::setPrimitive(const Object::Primitive::Base *primitive)
{
	mPrimitive = primitive;
}

double Intersection::distance() const
{
	return mDistance;
}

void Intersection::setDistance(double distance)
{
	mDistance = distance;
}

const Math::Vector &Intersection::normal() const
{
	if(!mTransformed)
		doTransform();

	return mNormal;
}

const Math::Vector &Intersection::objectNormal() const
{
	return mObjectNormal;
}

void Intersection::setObjectNormal(const Math::Vector &objectNormal)
{
	mTransformed = false;

	mObjectNormal = objectNormal;
}

const Math::Point &Intersection::point() const
{
	if(!mTransformed)
		doTransform();

	return mPoint;
}

const Math::Point &Intersection::objectPoint() const
{
	return mObjectPoint;
}

void Intersection::setObjectPoint(const Math::Point &objectPoint) 
{
	mTransformed = false;

	mObjectPoint = objectPoint;
}

void Intersection::transform(const Math::Transformation &transformation)
{
	if(mCompositeTransformed)
	{
		mTransformation = transformation.transformTransformation(mTransformation);
	}
	else
	{
		mTransformation = transformation.transformTransformation(mPrimitive->transformation());
		mCompositeTransformed = true;
	}

	if(mTransformed)
	{
		mNormal = transformation.transformNormal(mNormal);
		mPoint = transformation.transformPoint(mPoint);
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
		mNormal = mTransformation.transformNormal(mObjectNormal);
		mPoint = mTransformation.transformPoint(mObjectPoint);
	}
	else
	{
		mNormal = mPrimitive->transformation().transformNormal(mObjectNormal);
		mPoint = mPrimitive->transformation().transformPoint(mObjectPoint);
	}

	mTransformed = true;
}

}
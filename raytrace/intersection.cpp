#include "intersection.h"

#include "primitive.h"

Intersection::Intersection()
{
	mValid = false;
}

Intersection::Intersection(const Primitive *primitive, double distance, const Vector &objectNormal, const Vector &objectPoint)
{
	mValid = true;
	mPrimitive = primitive;
	mDistance = distance;
	mObjectNormal = objectNormal;
	mObjectPoint = objectPoint;
	mTransformed = false;
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

const Primitive *Intersection::primitive() const
{
	return mPrimitive;
}

void Intersection::setPrimitive(const Primitive *primitive)
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

const Vector &Intersection::normal() const
{
	if(!mTransformed)
		transform();

	return mNormal;
}

const Vector &Intersection::objectNormal() const
{
	return mObjectNormal;
}

void Intersection::setObjectNormal(const Vector &objectNormal)
{
	mTransformed = false;

	mObjectNormal = objectNormal;
}

const Vector &Intersection::point() const
{
	if(!mTransformed)
		transform();

	return mPoint;
}

const Vector &Intersection::objectPoint() const
{
	return mObjectPoint;
}

void Intersection::setObjectPoint(const Vector &objectPoint) 
{
	mTransformed = false;

	mObjectPoint = objectPoint;
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

void Intersection::transform() const
{
	mNormal = mPrimitive->transformation().transformNormal(mObjectNormal);
	mPoint = mPrimitive->transformation().transformPoint(mObjectPoint);

	mTransformed = true;
}

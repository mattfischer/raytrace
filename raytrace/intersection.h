#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "vector.h"

class Primitive;

class Intersection
{
public:
	Intersection();
	Intersection(const Primitive *primitive, double distance, const Vector &objectNormal, const Vector &objectPoint);
	Intersection(const Intersection &c);
	Intersection &operator=(const Intersection &c);

	bool valid() const;
	void setValid(bool valid);

	const Primitive *primitive() const;
	void setPrimitive(const Primitive *primitive);

	double distance() const;
	void setDistance(double distance);

	const Vector &normal() const;

	const Vector &objectNormal() const;
	void setObjectNormal(const Vector &objectNormal);

	const Vector &point() const;

	const Vector &objectPoint() const;
	void setObjectPoint(const Vector &objectPoint);

	const Intersection &nearest(const Intersection &b) const;
	
	bool operator<(const Intersection &b) const;

protected:
	bool mValid;
	const Primitive *mPrimitive;
	double mDistance;
	Vector mObjectNormal;
	Vector mObjectPoint;
	mutable Vector mPoint;
	mutable Vector mNormal;
	mutable bool mTransformed;

	void transform() const;
};

#endif

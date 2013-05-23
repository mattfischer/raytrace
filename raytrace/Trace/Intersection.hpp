#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "Math/Normal.hpp"
#include "Math/Transformation.hpp"

namespace Object {
namespace Primitive {
class Base;
}
}

namespace Trace {

class Intersection
{
public:
	Intersection();
	Intersection(const Object::Primitive::Base *primitive, float distance, const Math::Normal &objectNormal, const Math::Point &objectPoint);
	Intersection(const Intersection &c);
	Intersection &operator=(const Intersection &c);

	bool valid() const;
	void setValid(bool valid);

	const Object::Primitive::Base *primitive() const;
	void setPrimitive(const Object::Primitive::Base *primitive);

	float distance() const;
	void setDistance(float distance);

	const Math::Normal &normal() const;

	const Math::Normal &objectNormal() const;
	void setObjectNormal(const Math::Normal &objectNormal);

	const Math::Point &point() const;

	const Math::Point &objectPoint() const;
	void setObjectPoint(const Math::Point &objectPoint);

	const Intersection &nearest(const Intersection &b) const;
	
	void transform(const Math::Transformation &transformation);
	const Math::Transformation &transformation() const;

	bool operator<(const Intersection &b) const;

protected:
	bool mValid;
	const Object::Primitive::Base *mPrimitive;
	float mDistance;
	Math::Normal mObjectNormal;
	Math::Point mObjectPoint;
	mutable Math::Point mPoint;
	mutable Math::Normal mNormal;
	mutable bool mTransformed;
	Math::Transformation mTransformation;
	bool mCompositeTransformed;

	void doTransform() const;
};

inline bool Intersection::valid() const
{
	return mValid;
}

inline void Intersection::setValid(bool valid)
{
	mValid = valid;
}

inline const Object::Primitive::Base *Intersection::primitive() const
{
	return mPrimitive;
}

inline void Intersection::setPrimitive(const Object::Primitive::Base *primitive)
{
	mPrimitive = primitive;
}

inline float Intersection::distance() const
{
	return mDistance;
}

inline void Intersection::setDistance(float distance)
{
	mDistance = distance;
}

inline const Math::Normal &Intersection::objectNormal() const
{
	return mObjectNormal;
}

inline void Intersection::setObjectNormal(const Math::Normal &objectNormal)
{
	mTransformed = false;

	mObjectNormal = objectNormal;
}

inline const Math::Point &Intersection::objectPoint() const
{
	return mObjectPoint;
}

inline void Intersection::setObjectPoint(const Math::Point &objectPoint) 
{
	mTransformed = false;

	mObjectPoint = objectPoint;
}

}
#endif

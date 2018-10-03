#ifndef OBJECT_INTERSECTION_HPP
#define OBJECT_INTERSECTION_HPP

#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Transformation.hpp"
#include "Math/Ray.hpp"

#include <vector>

namespace Object {

namespace Primitive {
	class Base;
}

class Intersection
{
public:
	Intersection();
	Intersection(const Object::Primitive::Base *primitive, const Math::Ray &ray, float distance, const Math::Normal &objectNormal, const Math::Point &objectPoint);
	Intersection(const Intersection &c);
	Intersection &operator=(const Intersection &c);

	bool valid() const;

	const Object::Primitive::Base *primitive() const;

	float distance() const;
	const Math::Ray &ray() const;
	const Math::Normal &normal() const;
	const Math::Normal &objectNormal() const;
	const Math::Point &point() const;
	const Math::Point &objectPoint() const;

	const Intersection &nearest(const Intersection &b) const;
	
	bool operator<(const Intersection &b) const;

protected:
	bool mValid;
	const Object::Primitive::Base *mPrimitive;
	Math::Ray mRay;
	float mDistance;
	Math::Normal mObjectNormal;
	Math::Point mObjectPoint;
	mutable Math::Point mPoint;
	mutable Math::Normal mNormal;
	mutable bool mTransformed;

	void doTransform() const;
};

}
#endif

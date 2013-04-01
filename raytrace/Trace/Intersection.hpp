#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"

namespace Object {
class Primitive;
}

namespace Trace {

class Intersection
{
public:
	Intersection();
	Intersection(const Object::Primitive *primitive, double distance, const Math::Vector &objectNormal, const Math::Vector &objectPoint);
	Intersection(const Intersection &c);
	Intersection &operator=(const Intersection &c);

	bool valid() const;
	void setValid(bool valid);

	const Object::Primitive *primitive() const;
	void setPrimitive(const Object::Primitive *primitive);

	double distance() const;
	void setDistance(double distance);

	const Math::Vector &normal() const;

	const Math::Vector &objectNormal() const;
	void setObjectNormal(const Math::Vector &objectNormal);

	const Math::Vector &point() const;

	const Math::Vector &objectPoint() const;
	void setObjectPoint(const Math::Vector &objectPoint);

	const Intersection &nearest(const Intersection &b) const;
	
	void transform(const Math::Transformation &transformation);
	const Math::Transformation &transformation() const;

	bool operator<(const Intersection &b) const;

protected:
	bool mValid;
	const Object::Primitive *mPrimitive;
	double mDistance;
	Math::Vector mObjectNormal;
	Math::Vector mObjectPoint;
	mutable Math::Vector mPoint;
	mutable Math::Vector mNormal;
	mutable bool mTransformed;
	Math::Transformation mTransformation;
	bool mCompositeTransformed;

	void doTransform() const;
};

}
#endif

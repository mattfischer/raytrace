#ifndef OBJECT_INTERSECTION_HPP
#define OBJECT_INTERSECTION_HPP

#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Transformation.hpp"
#include "Math/Ray.hpp"

#include "Object/Primitive.hpp"

#include <vector>

namespace Object {

class Intersection
{
public:
	Intersection();
	Intersection(const Object::Primitive &primitive, const Math::Ray &ray, float distance, const Math::Normal &normal);

	bool valid() const;

	const Object::Primitive &primitive() const;

	float distance() const;
	const Math::Ray &ray() const;
	const Math::Normal &normal() const;
	Math::Point point() const;

protected:
	const Object::Primitive &mPrimitive;
	const Math::Ray &mRay;
	float mDistance;
	Math::Normal mNormal;
};

}
#endif

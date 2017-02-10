#ifndef OBJECT_PRIMITIVE_BOUNDING_SPHERE_HPP
#define OBJECT_PRIMITIVE_BOUNDING_SPHERE_HPP

#include "Math/Point.hpp"

namespace Object {
namespace Primitive {

class BoundingSphere {
public:
	BoundingSphere() = default;
	BoundingSphere(const Math::Point &origin, float radius);

	const Math::Point &origin() const;
	float radius() const;

private:
	Math::Point mOrigin;
	float mRadius;
};

}
}
#endif
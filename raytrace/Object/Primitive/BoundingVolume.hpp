#ifndef OBJECT_PRIMITIVE_BOUNDING_VOLUME_HPP
#define OBJECT_PRIMITIVE_BOUNDING_VOLUME_HPP

#include "Math/Vector.hpp"
#include "Math/Ray.hpp"

#include <array>

namespace Object {
namespace Primitive {

class BoundingVolume
{
public:
	static const int NUM_VECTORS = 3;
	struct RayData {
		float offsets[NUM_VECTORS];
		float dots[NUM_VECTORS];
	};

	BoundingVolume() = default;
	BoundingVolume(const float mins[NUM_VECTORS], const float maxes[NUM_VECTORS]);

	BoundingVolume translate(const Math::Vector &translate);

	bool intersectRay(const RayData &rayData) const;

	static const std::array<Math::Vector, NUM_VECTORS> &vectors();
	static RayData getRayData(const Math::Ray &ray);

private:
	float mMins[NUM_VECTORS];
	float mMaxes[NUM_VECTORS];
};

}
}
#endif
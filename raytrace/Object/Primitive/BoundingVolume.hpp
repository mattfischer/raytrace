#ifndef OBJECT_PRIMITIVE_BOUNDING_VOLUME_HPP
#define OBJECT_PRIMITIVE_BOUNDING_VOLUME_HPP

#include "Math/Vector.hpp"
#include "Math/Ray.hpp"

#include <vector>

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
	BoundingVolume(const std::vector<float> &mins, const std::vector<float> &maxes);

	BoundingVolume translate(const Math::Vector &translate);

	bool intersectRay(const RayData &rayData) const;

	static const std::vector<Math::Vector> &vectors();
	static RayData getRayData(const Math::Ray &ray);

private:
	std::vector<float> mMins;
	std::vector<float> mMaxes;
};

}
}
#endif
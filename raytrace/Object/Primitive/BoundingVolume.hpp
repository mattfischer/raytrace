#ifndef OBJECT_PRIMITIVE_BOUNDING_VOLUME_HPP
#define OBJECT_PRIMITIVE_BOUNDING_VOLUME_HPP

#include "Math/Vector.hpp"

#include <vector>

namespace Object {
namespace Primitive {

class BoundingVolume
{
public:
	BoundingVolume() = default;
	BoundingVolume(const std::vector<float> &mins, const std::vector<float> &maxes);

	BoundingVolume translate(const Math::Vector &translate);

	static const std::vector<Math::Vector> &vectors();

private:
	std::vector<float> mMins;
	std::vector<float> mMaxes;
};

}
}
#endif
#include "BoundingVolume.hpp"

#include <algorithm>

namespace Object {
namespace Primitive {

std::array<Math::Vector, BoundingVolume::NUM_VECTORS> sVectors{ Math::Vector(1, 0, 0), Math::Vector(0, 1, 0), Math::Vector(0, 0, 1) };

BoundingVolume::BoundingVolume(const float mins[NUM_VECTORS], const float maxes[NUM_VECTORS])
{
	for (int i = 0; i < NUM_VECTORS; i++) {
		mMins[i] = mins[i];
		mMaxes[i] = maxes[i];
	}
}

const std::array<Math::Vector, BoundingVolume::NUM_VECTORS> &BoundingVolume::vectors()
{
	return sVectors;
}

BoundingVolume BoundingVolume::translate(const Math::Vector &translate)
{
	float mins[NUM_VECTORS];
	float maxes[NUM_VECTORS];

	for(int i=0; i<NUM_VECTORS; i++) {
		const Math::Vector &vector = sVectors[i];
		float offset = vector * translate;
		mins[i] = mMins[i] + offset;
		maxes[i] = mMaxes[i] + offset;
	}

	return BoundingVolume(mins, maxes);
}

bool BoundingVolume::intersectRay(const RayData &rayData) const
{
	float minDist = -FLT_MAX;
	float maxDist = FLT_MAX;

	for (int i = 0; i < NUM_VECTORS; i++) {
		float offset = rayData.offsets[i];
		float dot = rayData.dots[i];

		float min;
		float max;

		if (dot < 0) {
			max = (mMins[i] - offset) / dot;
			min = (mMaxes[i] - offset) / dot;
		}
		else if (dot == 0) {
			if (offset > mMaxes[i] || offset < mMins[i]) {
				return false;
			}
			else {
				continue;
			}
		}
		else {
			min = (mMins[i] - offset) / dot;
			max = (mMaxes[i] - offset) / dot;
		}

		minDist = std::max(minDist, min);
		maxDist = std::min(maxDist, max);

		if (minDist > maxDist || maxDist < 0) {
			return false;
		}
	}

	return (minDist <= maxDist && maxDist >= 0);
}

BoundingVolume::RayData BoundingVolume::getRayData(const Math::Ray &ray)
{
	RayData rayData;

	for (int i = 0; i < NUM_VECTORS; i++) {
		const Math::Vector &vector = sVectors[i];
		rayData.offsets[i] = Math::Vector(ray.origin()) * vector;
		rayData.dots[i] = ray.direction() * vector;
	}

	return rayData;
}

}
}
#include "BoundingVolume.hpp"

#include <algorithm>

namespace Object {
namespace Primitive {

std::vector<Math::Vector> sVectors{ Math::Vector(1, 0, 0), Math::Vector(0, 1, 0), Math::Vector(0, 0, 1) };

BoundingVolume::BoundingVolume(const std::vector<float> &mins, const std::vector<float> &maxes)
	: mMins(mins), mMaxes(maxes)
{
}

const std::vector<Math::Vector> &BoundingVolume::vectors()
{
	return sVectors;
}

BoundingVolume::RayData BoundingVolume::getRayData(const Trace::Ray &ray)
{
	RayData rayData;
	for (const Math::Vector &vector : vectors()) {
		rayData.offsets.push_back(Math::Vector(ray.origin()) * vector);
		rayData.invdots.push_back(1 / (ray.direction() * vector));
	}

	return rayData;
}

BoundingVolume BoundingVolume::translate(const Math::Vector &translate)
{
	std::vector<float> mins;
	std::vector<float> maxes;

	for(int i=0; i<vectors().size(); i++) {
		const Math::Vector &vector = vectors()[i];
		float offset = vector * translate;
		mins.push_back(mMins[i] + offset);
		maxes.push_back(mMaxes[i] + offset);
	}

	return BoundingVolume(mins, maxes);
}

bool BoundingVolume::intersectRay(const RayData &rayData) const
{
	float minDist = -FLT_MAX;
	float maxDist = FLT_MAX;

	for (int i = 0; i < vectors().size(); i++) {
		const Math::Vector &vector = vectors()[i];
		float offset = rayData.offsets[i];
		float invdot = rayData.invdots[i];

		if (std::isnan(invdot)) {
			continue;
		}

		float min = (mMins[i] - offset) * invdot;
		float max = (mMaxes[i] - offset) * invdot;

		if (invdot < 0) {
			std::swap(min, max);
		}

		minDist = std::max(minDist, min);
		maxDist = std::min(maxDist, max);
	}

	return (minDist <= maxDist && maxDist > 0);
}

}
}
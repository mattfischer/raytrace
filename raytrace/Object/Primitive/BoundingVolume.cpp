#include "BoundingVolume.hpp"

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

}
}
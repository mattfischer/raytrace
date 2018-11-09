#ifndef OBJECT_BOUNDING_VOLUME_HIERARCHY_HPP
#define OBJECT_BOUNDING_VOLUME_HIERARCHY_HPP

#include "Object/BoundingVolume.hpp"

#include <vector>
#include <functional>

namespace Object {
	class BoundingVolumeHierarchy
	{
	public:
		struct Node {
			BoundingVolume volume;
			int index;
		};

		BoundingVolumeHierarchy() = default;
		BoundingVolumeHierarchy(std::vector<Node> &&nodes);

		bool intersect(const BoundingVolume::RayData &rayData, float &maxDistance, const std::function<bool(int, float&)> &func) const;

	private:
		bool intersectNode(const BoundingVolume::RayData &rayData, int nodeIndex, float &maxDistance, const std::function<bool(int, float&)> &func) const;

		std::vector<Node> mNodes;
	};
}
#endif
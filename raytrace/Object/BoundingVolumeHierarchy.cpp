#include "Object/BoundingVolumeHierarchy.hpp"

namespace Object {
	BoundingVolumeHierarchy::BoundingVolumeHierarchy(std::vector<Node> &&nodes)
		: mNodes(std::move(nodes))
	{
	}

	const std::vector<BoundingVolumeHierarchy::Node> &BoundingVolumeHierarchy::nodes() const
	{
		return mNodes;
	}

	bool BoundingVolumeHierarchy::intersect(const BoundingVolume::RayData &rayData, float &maxDistance, const std::function<bool(int, float&)> &func) const
	{
		return intersectNode(rayData, 0, maxDistance, func);
	}

	bool BoundingVolumeHierarchy::intersectNode(const BoundingVolume::RayData &rayData, int nodeIndex, float &maxDistance, const std::function<bool(int, float&)> &func) const
	{
		const Node &node = mNodes[nodeIndex];

		bool ret = false;
		if (node.index <= 0) {
			int index = -node.index;
			if (func(index, maxDistance)) {
				ret = true;
			}
		}
		else {
			int indices[2] = { nodeIndex + 1, node.index };
			float distances[2];
			for (int i = 0; i < 2; i++) {
				distances[i] = FLT_MAX;
				mNodes[indices[i]].volume.intersectRay(rayData, distances[i]);
			}

			for (int i = 0; i < 2; i++) {
				int j = (distances[0] < distances[1]) ? i : 1 - i;
				if (distances[j] < maxDistance && intersectNode(rayData, indices[j], maxDistance, func)) {
					ret = true;
				}
			}
		}

		return ret;
	}
}
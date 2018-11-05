#include "Object/BoundingVolumeHierarchy.hpp"

namespace Object {
	BoundingVolumeHierarchy::BoundingVolumeHierarchy(std::unique_ptr<Node> root)
		: mRoot(std::move(root))
	{
	}

	bool BoundingVolumeHierarchy::intersect(const BoundingVolume::RayData &rayData, float &maxDistance, const std::function<bool(const Node&, float&)> &func) const
	{
		return intersectNode(rayData, *mRoot, maxDistance, func);
	}

	bool BoundingVolumeHierarchy::intersectNode(const BoundingVolume::RayData &rayData, const Node &node, float &maxDistance, const std::function<bool(const Node&, float&)> &func) const
	{
		bool ret = false;
		if (!node.children[0] && !node.children[1]) {
			if (func(node, maxDistance)) {
				ret = true;
			}
		}
		else {
			float distances[2];
			for (int i = 0; i < 2; i++) {
				distances[i] = FLT_MAX;
				node.children[i]->volume.intersectRay(rayData, distances[i]);
			}

			for (int i = 0; i < 2; i++) {
				int j = (distances[0] < distances[1]) ? i : 1 - i;
				if (distances[j] < maxDistance && intersectNode(rayData, *node.children[j], maxDistance, func)) {
					ret = true;
				}
			}
		}

		return ret;
	}
}
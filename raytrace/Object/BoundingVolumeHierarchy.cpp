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
			int indices[2];
			int numIndices = 0;
			for (int i = 0; i < 2; i++) {
				if (node.children[i]->volume.intersectRay(rayData, distances[i]) && distances[i] < maxDistance) {
					indices[numIndices] = i;
					numIndices++;
					for (int j = 0; j < numIndices - 1; j++) {
						if (distances[indices[numIndices - 1]] < distances[indices[j]]) {
							std::swap(indices[numIndices - 1], indices[j]);
						}
					}
				}
			}

			for (int i = 0; i < numIndices; i++) {
				if (distances[indices[i]] < maxDistance && intersectNode(rayData, *node.children[indices[i]], maxDistance, func)) {
					ret = true;
				}
			}
		}

		return ret;
	}
}
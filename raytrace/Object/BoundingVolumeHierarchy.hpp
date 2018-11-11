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
		BoundingVolumeHierarchy(const std::vector<Math::Point> &points, const std::function<BoundingVolume(int)> &func);

		bool intersect(const BoundingVolume::RayData &rayData, float &maxDistance, const std::function<bool(int, float&)> &func) const;

		const std::vector<Node> &nodes() const;

	private:
		bool intersectNode(const BoundingVolume::RayData &rayData, int nodeIndex, float &maxDistance, const std::function<bool(int, float&)> &func) const;

		struct TreeNode {
			int index;
		};
		int buildKdTree(const std::vector<Math::Point> &points, std::vector<TreeNode> &tree, std::vector<int>::iterator indicesBegin, std::vector<int>::iterator indicesEnd, int splitIndex) const;
		int computeBounds(const std::vector<TreeNode> &tree, const std::function<BoundingVolume(int)> &func, int index);

		std::vector<Node> mNodes;
	};
}
#endif
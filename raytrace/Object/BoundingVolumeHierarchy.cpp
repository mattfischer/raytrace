#include "Object/BoundingVolumeHierarchy.hpp"

namespace Object {
	const Math::Vector splitPlanes[3] = { Math::Vector(1, 0, 0), Math::Vector(0, 1, 0), Math::Vector(0, 0, 1) };

	BoundingVolumeHierarchy::BoundingVolumeHierarchy(std::vector<Node> &&nodes)
		: mNodes(std::move(nodes))
	{
	}

	BoundingVolumeHierarchy::BoundingVolumeHierarchy(const std::vector<Math::Point> &points, const std::function<BoundingVolume(int)> &func)
	{
		std::vector<int> indices(points.size());
		for (unsigned int i = 0; i < indices.size(); i++) {
			indices[i] = i;
		}

		std::vector<TreeNode> tree;
		tree.reserve(points.size() * 2);
		buildKdTree(points, tree, indices.begin(), indices.end(), 0);

		mNodes.reserve(points.size() * 2);
		computeBounds(tree, func, 0);
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

	int BoundingVolumeHierarchy::buildKdTree(const std::vector<Math::Point> &centroids, std::vector<TreeNode> &tree, std::vector<int>::iterator indicesBegin, std::vector<int>::iterator indicesEnd, int splitIndex) const
	{
		tree.push_back(TreeNode());
		int nodeIndex = tree.size() - 1;
		TreeNode &node = tree[nodeIndex];

		int numIndices = indicesEnd - indicesBegin;
		if (numIndices == 1) {
			int triangleIndex = *indicesBegin;
			node.index = -triangleIndex;
		}
		else {
			const Math::Vector &splitPlane = splitPlanes[splitIndex];

			auto pointDistance = [&](const int &idx0, const int &idx1) {
				return Math::Vector(centroids[idx0]) * splitPlane < Math::Vector(centroids[idx1]) * splitPlane;
			};

			std::vector<int>::iterator minIt = std::min_element(indicesBegin, indicesEnd, pointDistance);
			std::vector<int>::iterator maxIt = std::max_element(indicesBegin, indicesEnd, pointDistance);

			float min = Math::Vector(centroids[*minIt]) * splitPlane;
			float max = Math::Vector(centroids[*maxIt]) * splitPlane;
			float pivot = (min + max) / 2;

			auto belowPivot = [&](const int &idx) {
				return Math::Vector(centroids[idx]) * splitPlane < pivot;
			};

			std::vector<int>::iterator split = std::stable_partition(indicesBegin, indicesEnd, belowPivot);
			if (split == indicesBegin) {
				split++;
			}
			else if (split == indicesEnd) {
				split--;
			}

			buildKdTree(centroids, tree, indicesBegin, split, (splitIndex + 1) % 3);
			node.index = buildKdTree(centroids, tree, split, indicesEnd, (splitIndex + 1) % 3);
		}

		return nodeIndex;
	}

	int BoundingVolumeHierarchy::computeBounds(const std::vector<TreeNode> &tree, const std::function<BoundingVolume(int)> &func, int treeIndex)
	{
		const TreeNode &treeNode = tree[treeIndex];
		mNodes.push_back(Object::BoundingVolumeHierarchy::Node());
		int nodeIndex = mNodes.size() - 1;
		Object::BoundingVolumeHierarchy::Node &node = mNodes[nodeIndex];

		if (treeNode.index <= 0) {
			int index = -treeNode.index;

			node.index = -index;
			node.volume = func(index);
		}
		else {
			computeBounds(tree, func, treeIndex + 1);
			node.volume.expand(mNodes[nodeIndex + 1].volume);
			node.index = computeBounds(tree, func, treeNode.index);
			node.volume.expand(mNodes[node.index].volume);
		}

		return nodeIndex;
	}
}
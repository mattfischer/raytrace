#ifndef OBJECT_BOUNDING_VOLUME_HIERARCHY_HPP
#define OBJECT_BOUNDING_VOLUME_HIERARCHY_HPP

#include "Object/BoundingVolume.hpp"

#include <memory>
#include <functional>

namespace Object {
	class BoundingVolumeHierarchy
	{
	public:
		struct Node {
			BoundingVolume volume;
			std::unique_ptr<Node> children[2];
		};

		BoundingVolumeHierarchy() = default;
		BoundingVolumeHierarchy(std::unique_ptr<Node> root);

		bool intersect(const BoundingVolume::RayData &rayData, float &maxDistance, const std::function<bool(const Node&, float&)> &func) const;

	private:
		bool intersectNode(const BoundingVolume::RayData &rayData, const Node &node, float &maxDistance, const std::function<bool(const Node&, float&)> &func) const;

		std::unique_ptr<Node> mRoot;
	};
}
#endif
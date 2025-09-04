#ifndef OBJECT_BOUNDING_VOLUME_HIERARCHY_HPP
#define OBJECT_BOUNDING_VOLUME_HIERARCHY_HPP

#include "Object/BoundingVolume.hpp"

#include <vector>
#include <functional>

#include "Object/Impl/Shape/CLProxies.hpp"

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
        BoundingVolumeHierarchy(const std::vector<Math::Point> &points, const std::function<BoundingVolume(unsigned int)> &func);

        bool intersect(const BoundingVolume::RayData &rayData, float &maxDistance, bool closest, const std::function<bool(unsigned int, float&)> &func) const;

        void writeProxy(BVHNodeProxy *proxy) const;

        const std::vector<Node> &nodes() const;

    private:
        struct TreeNode {
            int index;
        };
        unsigned int buildKdTree(const std::vector<Math::Point> &points, std::vector<TreeNode> &tree, std::vector<unsigned int>::iterator indicesBegin, std::vector<unsigned int>::iterator indicesEnd, unsigned int splitIndex) const;
        unsigned int computeBounds(const std::vector<TreeNode> &tree, const std::function<BoundingVolume(unsigned int)> &func, unsigned int index);

        std::vector<Node> mNodes;
    };
}
#endif

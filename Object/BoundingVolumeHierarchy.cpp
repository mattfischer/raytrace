#include "Object/BoundingVolumeHierarchy.hpp"
#include <cfloat>
#include <algorithm>

namespace Object {
    const Math::Vector splitPlanes[3] = { Math::Vector(1, 0, 0), Math::Vector(0, 1, 0), Math::Vector(0, 0, 1) };

    BoundingVolumeHierarchy::BoundingVolumeHierarchy(std::vector<Node> &&nodes)
        : mNodes(std::move(nodes))
    {
    }

    BoundingVolumeHierarchy::BoundingVolumeHierarchy(const std::vector<Math::Point> &points, const std::function<BoundingVolume(unsigned int)> &func)
    {
        std::vector<unsigned int> indices(points.size());
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

    bool BoundingVolumeHierarchy::intersect(const BoundingVolume::RayData &rayData, float &maxDistance, bool closest, const std::function<bool(unsigned int, float&)> &func) const
    {
        struct StackEntry {
            int nodeIndex;
            float minDistance;
        };

        StackEntry stack[64];

        bool ret = false;
        int n = 0;
        stack[n].nodeIndex = 0;
        stack[n].minDistance = 0;
        n++;
        do {
            n--;
            int nodeIndex = stack[n].nodeIndex;
            const Node &node = mNodes[nodeIndex];
            float nodeMinimum = stack[n].minDistance;
            
            if(nodeMinimum > maxDistance) {
                continue;
            }

            if (node.index <= 0) {
                int index = -node.index;
                if (func(index, maxDistance)) {
                    ret = true;
                    if(!closest) {
                        break;
                    }
                }
            }
            else {
                unsigned int indices[2] = { nodeIndex + 1, static_cast<unsigned int>(node.index) };
                float minDistances[2];
                float maxDistances[2];
                for (int i = 0; i < 2; i++) {
                    minDistances[i] = FLT_MAX;
                    maxDistances[i] = -FLT_MAX;
                    mNodes[indices[i]].volume.intersectRay(rayData, minDistances[i], maxDistances[i]);
                }

                for (int i = 0; i < 2; i++) {
                    int j = (minDistances[0] >= minDistances[1]) ? i : 1 - i;
                    if(maxDistances[j] > 0) {
                        stack[n].nodeIndex = indices[j];
                        stack[n].minDistance = minDistances[j];
                        n++;
                    }
                }
            }
        } while(n > 0);

        return ret;
    }

    unsigned int BoundingVolumeHierarchy::buildKdTree(const std::vector<Math::Point> &centroids, std::vector<TreeNode> &tree, std::vector<unsigned int>::iterator indicesBegin, std::vector<unsigned int>::iterator indicesEnd, unsigned int splitIndex) const
    {
        tree.push_back(TreeNode());
        unsigned int nodeIndex = static_cast<unsigned int>(tree.size() - 1);
        TreeNode &node = tree[nodeIndex];

        unsigned int numIndices = static_cast<unsigned int>(indicesEnd - indicesBegin);
        if (numIndices == 1) {
            unsigned int triangleIndex = *indicesBegin;
            node.index = -static_cast<int>(triangleIndex);
        }
        else {
            const Math::Vector &splitPlane = splitPlanes[splitIndex];

            auto cmp = [&](const unsigned int &idx0, const unsigned int &idx1) {
                return Math::Vector(centroids[idx0]) * splitPlane < Math::Vector(centroids[idx1]) * splitPlane;
            };

            std::vector<unsigned int>::iterator split = indicesBegin + (indicesEnd - indicesBegin) / 2;
            std::nth_element(indicesBegin, split, indicesEnd, cmp);

            buildKdTree(centroids, tree, indicesBegin, split, (splitIndex + 1) % 3);
            node.index = static_cast<int>(buildKdTree(centroids, tree, split, indicesEnd, (splitIndex + 1) % 3));
        }

        return nodeIndex;
    }

    unsigned int BoundingVolumeHierarchy::computeBounds(const std::vector<TreeNode> &tree, const std::function<BoundingVolume(unsigned int)> &func, unsigned int treeIndex)
    {
        const TreeNode &treeNode = tree[treeIndex];
        mNodes.push_back(Object::BoundingVolumeHierarchy::Node());
        unsigned int nodeIndex = static_cast<unsigned int>(mNodes.size() - 1);
        Object::BoundingVolumeHierarchy::Node &node = mNodes[nodeIndex];

        if (treeNode.index <= 0) {
            unsigned int index = static_cast<unsigned int>(-treeNode.index);

            node.index = -static_cast<int>(index);
            node.volume = func(index);
        }
        else {
            computeBounds(tree, func, treeIndex + 1);
            node.volume.expand(mNodes[nodeIndex + 1].volume);
            node.index = computeBounds(tree, func, static_cast<unsigned int>(treeNode.index));
            node.volume.expand(mNodes[static_cast<unsigned int>(node.index)].volume);
        }

        return nodeIndex;
    }

    void BoundingVolumeHierarchy::writeProxy(BVHNodeProxy *proxy) const
    {
        for(int i=0; i<mNodes.size(); i++) {
            proxy[i].index = mNodes[i].index;
            mNodes[i].volume.writeProxy(proxy[i].volume);
        }
    }
}

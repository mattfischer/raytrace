#include "Object/Shape/TriangleMesh.hpp"
#include "Object/Shape/Triangle.hpp"

#include <algorithm>

namespace Object {
	namespace Shape {
		const Math::Vector splitPlanes[3] = { Math::Vector(1, 0, 0), Math::Vector(0, 1, 0), Math::Vector(0, 0, 1) };

		TriangleMesh::TriangleMesh(std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles)
			: mVertices(std::move(vertices)), mTriangles(std::move(triangles))
		{
			std::vector<Math::Point> centroids(mTriangles.size());
			for (unsigned int i = 0; i < mTriangles.size(); i++) {
				const Triangle &triangle = mTriangles[i];
				Math::Vector centroid;
				for (unsigned int j = 0; j < 3; j++) {
					centroid = centroid + Math::Vector(mVertices[triangle.vertices[j]].point);
				}
				centroids[i] = Math::Point(centroid / 3.0f);
			}

			std::vector<int> indices(mTriangles.size());
			for (unsigned int i = 0; i < indices.size(); i++) {
				indices[i] = i;
			}

			std::vector<TreeNode> tree;
			tree.reserve(centroids.size() * 2);
			buildKdTree(centroids, tree, indices.begin(), indices.end(), 0);

			std::vector<Object::BoundingVolumeHierarchy::Node> bvhNodes;
			bvhNodes.reserve(centroids.size() * 2);
			computeBounds(bvhNodes, tree, 0);
			mBoundingVolumeHierarchy = Object::BoundingVolumeHierarchy(std::move(bvhNodes));
		}

		bool TriangleMesh::intersect(const Math::Ray &ray, Intersection &intersection) const
		{
			BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);

			auto callback = [&](int index, float &maxDistance) {
				const Triangle &triangle = mTriangles[index];
				bool ret = false;

				const Vertex &vertex0 = mVertices[triangle.vertices[0]];
				const Vertex &vertex1 = mVertices[triangle.vertices[1]];
				const Vertex &vertex2 = mVertices[triangle.vertices[2]];

				float tu, tv;
				if (Object::Shape::Triangle::intersect(ray, vertex0.point, vertex1.point, vertex2.point, intersection.distance, tu, tv)) {
					intersection.normal = triangle.normal;
					intersection.tangent = Math::Bivector(Math::Vector(), Math::Vector());
					intersection.surfacePoint = Math::Point2D();
					ret = true;
				}
				return ret;
			};

			return mBoundingVolumeHierarchy.intersect(rayData, intersection.distance, std::ref(callback));
		}

		BoundingVolume TriangleMesh::boundingVolume(const Math::Transformation &transformation) const
		{
			BoundingVolume volume;
			for (const Vertex &vertex : mVertices) {
				volume.expand(transformation * vertex.point);
			}

			return volume;
		}

		int TriangleMesh::buildKdTree(const std::vector<Math::Point> &centroids, std::vector<TreeNode> &tree, std::vector<int>::iterator indicesBegin, std::vector<int>::iterator indicesEnd, int splitIndex) const
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

		int TriangleMesh::computeBounds(std::vector<Object::BoundingVolumeHierarchy::Node> &nodes, const std::vector<TreeNode> &tree, int treeIndex) const
		{
			const TreeNode &treeNode = tree[treeIndex];
			nodes.push_back(Object::BoundingVolumeHierarchy::Node());
			int nodeIndex = nodes.size() - 1;
			Object::BoundingVolumeHierarchy::Node &node = nodes[nodeIndex];

			if (treeNode.index <= 0) {
				int index = -treeNode.index;
				node.index = -index;
				for (unsigned int i = 0; i < 3; i++) {
					node.volume.expand(mVertices[mTriangles[index].vertices[i]].point);
				}
			}
			else {
				computeBounds(nodes, tree, treeIndex + 1);
				node.volume.expand(nodes[nodeIndex + 1].volume);
				node.index = computeBounds(nodes, tree, treeNode.index);
				node.volume.expand(nodes[node.index].volume);
			}

			return nodeIndex;
		}
	}
}
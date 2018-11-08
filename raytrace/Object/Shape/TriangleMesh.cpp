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

			std::unique_ptr<TreeNode> root = buildKdTree(centroids, indices.begin(), indices.end(), 0);
			mBoundingVolumeHierarchy = Object::BoundingVolumeHierarchy(computeBounds(*root));
		}

		bool TriangleMesh::intersect(const Math::Ray &ray, Intersection &intersection) const
		{
			BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);

			auto callback = [&](const BoundingVolumeHierarchy::Node &node, float &maxDistance) {
				const BvhNode &bvhNode = static_cast<const BvhNode&>(node);
				const Triangle &triangle = mTriangles[bvhNode.index];
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

		std::unique_ptr<TriangleMesh::TreeNode> TriangleMesh::buildKdTree(const std::vector<Math::Point> &centroids, std::vector<int>::iterator indicesBegin, std::vector<int>::iterator indicesEnd, int splitIndex) const
		{
			std::unique_ptr<TreeNode> node = std::make_unique<TreeNode>();
			if (indicesEnd - indicesBegin == 1) {
				node->index = *indicesBegin;
			}
			else {
				const Math::Vector &splitPlane = splitPlanes[splitIndex];
				auto cmp = [&](const int &idx0, const int &idx1) {
					return Math::Vector(centroids[idx0]) * splitPlane < Math::Vector(centroids[idx1]) * splitPlane;
				};

				std::sort(indicesBegin, indicesEnd, cmp);

				int splitIdx = (indicesEnd - indicesBegin) / 2;
				node->children[0] = buildKdTree(centroids, indicesBegin, indicesBegin + splitIdx, (splitIndex + 1) % 3);
				node->children[1] = buildKdTree(centroids, indicesBegin + splitIdx, indicesEnd, (splitIndex + 1) % 3);
			}

			return node;
		}

		std::unique_ptr<Object::BoundingVolumeHierarchy::Node> TriangleMesh::computeBounds(const TreeNode &treeNode) const
		{
			if (!treeNode.children[0] && !treeNode.children[1]) {
				std::unique_ptr<BvhNode> bvhNode = std::make_unique<BvhNode>();
				bvhNode->index = treeNode.index;
				for (unsigned int i = 0; i < 3; i++) {
					bvhNode->volume.expand(mVertices[mTriangles[treeNode.index].vertices[i]].point);
				}
				return bvhNode;
			}
			else {
				std::unique_ptr<Object::BoundingVolumeHierarchy::Node> bvhNode = std::make_unique<Object::BoundingVolumeHierarchy::Node>();
				for (unsigned int i = 0; i < 2; i++) {
					bvhNode->children[i] = computeBounds(*treeNode.children[i]);
					bvhNode->volume.expand(bvhNode->children[i]->volume);
				}
				return bvhNode;
			}
		}
	}
}
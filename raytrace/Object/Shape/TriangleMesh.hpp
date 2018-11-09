#ifndef OBJECT_SHAPE_TRIANGLE_MESH_HPP
#define OBJECT_SHAPE_TRIANGLE_MESH_HPP

#include "Object/Shape/Base.hpp"
#include "Object/BoundingVolumeHierarchy.hpp"

#include "Math/Point.hpp"
#include "Math/Bivector.hpp"
#include "Math/Normal.hpp"

#include <vector>

namespace Object {
	namespace Shape {
		class TriangleMesh : public Base
		{
		public:
			struct Vertex {
				Math::Point point;
			};

			struct Triangle {
				int vertices[3];
				Math::Normal normal;
			};

			TriangleMesh(std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles);

			virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const;
			virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;

		private:
			struct TreeNode {
				int index;
			};

			int buildKdTree(const std::vector<Math::Point> &centroids, std::vector<TreeNode> &tree, std::vector<int>::iterator indicesBegin, std::vector<int>::iterator indicesEnd, int splitIndex) const;
			int computeBounds(std::vector<Object::BoundingVolumeHierarchy::Node> &bvh, const std::vector<TreeNode> &tree, int index) const;

			std::vector<Vertex> mVertices;
			std::vector<Triangle> mTriangles;
			Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
		};
	}
}
#endif
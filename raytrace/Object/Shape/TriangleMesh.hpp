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
				std::unique_ptr<TreeNode> children[2];
			};

			struct BvhNode : BoundingVolumeHierarchy::Node {
				int index;
			};

			std::unique_ptr<TreeNode> buildKdTree(const std::vector<Math::Point> &centroids, std::vector<int>::iterator indicesBegin, std::vector<int>::iterator indicesEnd, int splitIndex) const;
			std::unique_ptr<Object::BoundingVolumeHierarchy::Node> computeBounds(const TreeNode &node) const;

			std::vector<Vertex> mVertices;
			std::vector<Triangle> mTriangles;
			Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
		};
	}
}
#endif
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
                unsigned int vertices[3];
				Math::Normal normal;
			};

			TriangleMesh(std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles);
			TriangleMesh(std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles, Object::BoundingVolumeHierarchy &&boundingVolumeHierarchy);

			virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const;
			virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;

			const Object::BoundingVolumeHierarchy &boundingVolumeHierarchy() const;

		private:
            Object::BoundingVolumeHierarchy computeBoundingVolumeHierarchy() const;

			std::vector<Vertex> mVertices;
			std::vector<Triangle> mTriangles;
			Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
		};
	}
}
#endif

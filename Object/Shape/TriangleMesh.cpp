#include "Object/Shape/TriangleMesh.hpp"
#include "Object/Shape/Triangle.hpp"

#include <algorithm>

namespace Object {
	namespace Shape {
		TriangleMesh::TriangleMesh(std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles)
			: mVertices(std::move(vertices)), mTriangles(std::move(triangles)), mBoundingVolumeHierarchy(computeBoundingVolumeHierarchy())
		{
		}

		TriangleMesh::TriangleMesh(std::vector<Vertex> &&vertices, std::vector<Triangle> &&triangles, Object::BoundingVolumeHierarchy &&boundingVolumeHierarchy)
			: mVertices(std::move(vertices)), mTriangles(std::move(triangles)), mBoundingVolumeHierarchy(std::move(boundingVolumeHierarchy))
		{
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

		const Object::BoundingVolumeHierarchy &TriangleMesh::boundingVolumeHierarchy() const
		{
			return mBoundingVolumeHierarchy;
		}

		Object::BoundingVolumeHierarchy TriangleMesh::computeBoundingVolumeHierarchy() const
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

			auto func = [&](int index) {
				BoundingVolume volume;
				for (unsigned int i = 0; i < 3; i++) {
					volume.expand(mVertices[mTriangles[index].vertices[i]].point);
				}
				return volume;
			};

			return Object::BoundingVolumeHierarchy(centroids, std::ref(func));
		}
	}
}
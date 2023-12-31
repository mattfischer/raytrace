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

        bool TriangleMesh::intersect(const Math::Ray &ray, Intersection &isect, bool closest) const
        {
            BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);

            auto callback = [&](unsigned int index, float &) {
                const Triangle &triangle = mTriangles[index];
                bool ret = false;

                const Vertex &vertex0 = mVertices[triangle.vertices[0]];
                const Vertex &vertex1 = mVertices[triangle.vertices[1]];
                const Vertex &vertex2 = mVertices[triangle.vertices[2]];

                float tu, tv;
                if (Object::Shape::Triangle::intersect(ray, vertex0.point, vertex1.point, vertex2.point, isect.distance, tu, tv)) {
                    isect.normal = triangle.normal;
                    isect.tangent = Math::Bivector(Math::Vector(), Math::Vector());
                    isect.surfacePoint = Math::Point2D();
                    ret = true;
                }
                return ret;
            };

            return mBoundingVolumeHierarchy.intersect(rayData, isect.distance, closest, std::ref(callback));
        }

        BoundingVolume TriangleMesh::boundingVolume(const Math::Transformation &trans) const
        {
            BoundingVolume volume;
            for (const Vertex &vertex : mVertices) {
                volume.expand(trans * vertex.point);
            }

            return volume;
        }

        const Object::BoundingVolumeHierarchy &TriangleMesh::boundingVolumeHierarchy() const
        {
            return mBoundingVolumeHierarchy;
        }

        void TriangleMesh::writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const
        {
            proxy.type = ShapeProxy::Type::TriangleMesh;
            proxy.triangleMesh.vertices = clAllocator.allocateArray<PointProxy>(mVertices.size());
            for(int i=0; i<mVertices.size(); i++) {
                mVertices[i].point.writeProxy(proxy.triangleMesh.vertices[i]);
            }
            proxy.triangleMesh.triangles = clAllocator.allocateArray<TriangleProxy>(mTriangles.size());
            for(int i=0; i<mTriangles.size(); i++) {
                for(int j=0; j<3; j++) {
                    proxy.triangleMesh.triangles[i].vertices.values[j] = mTriangles[i].vertices[j];
                    mTriangles[i].normal.writeProxy(proxy.triangleMesh.triangles[i].normal);
                }
            }
            proxy.triangleMesh.bvh = clAllocator.allocateArray<BVHNodeProxy>(mBoundingVolumeHierarchy.nodes().size());
            mBoundingVolumeHierarchy.writeProxy(proxy.triangleMesh.bvh);
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

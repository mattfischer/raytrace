#ifndef OBJECT_IMPL_SHAPE_TRIANGLE_MESH_HPP
#define OBJECT_IMPL_SHAPE_TRIANGLE_MESH_HPP

#include "Object/Shape.hpp"
#include "Object/BoundingVolumeHierarchy.hpp"

#include "Math/Point.hpp"
#include "Math/Bivector.hpp"
#include "Math/Normal.hpp"

#include <vector>

namespace Object::Impl::Shape {
    class TriangleMesh : public Object::Shape
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

        bool intersect(const Math::Ray &ray, Intersection &isect, bool closest) const override;
        BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

        const Object::BoundingVolumeHierarchy &boundingVolumeHierarchy() const;

        void writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const override;

    private:
        Object::BoundingVolumeHierarchy computeBoundingVolumeHierarchy() const;

        std::vector<Vertex> mVertices;
        std::vector<Triangle> mTriangles;
        Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
    };
}
#endif

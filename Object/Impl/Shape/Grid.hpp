#ifndef OBJECT_IMPL_SHAPE_GRID_HPP
#define OBJECT_IMPL_SHAPE_GRID_HPP

#include "Object/Shape.hpp"
#include "Object/BoundingVolumeHierarchy.hpp"

#include "Math/Point.hpp"
#include "Math/Bivector.hpp"

#include <vector>
#include <functional>

namespace Object::Impl::Shape {
    class Grid : public Object::Shape
    {
    public:
        struct Vertex {
            Math::Point point;
            Math::Normal normal;
            Math::Bivector tangent;
        };

        Grid(unsigned int width, unsigned int height, std::vector<Vertex> &&vertices);

        bool intersect(const Math::Ray &ray, Intersection &isect, bool closest) const override;
        BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

        void writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const;

    private:
        unsigned int computeBounds(std::vector<Object::BoundingVolumeHierarchy::Node> &nodes, unsigned int u, unsigned int v, unsigned int du, unsigned int dv) const;
        const Vertex &vertex(unsigned int u, unsigned int v) const;

        unsigned int mWidth;
        unsigned int mHeight;
        std::vector<Vertex> mVertices;
        Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
    };
}
#endif

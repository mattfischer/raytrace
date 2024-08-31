#ifndef OBJECT_SHAPE_BEZIER_PATCH_HPP
#define OBJECT_SHAPE_BEZIER_PATCH_HPP

#include "Object/Shape/Base.hpp"
#include "Object/Shape/Grid.hpp"

#include <vector>

namespace Object::Shape {
    class BezierPatch : public Base {
    public:
        BezierPatch(int width, int height, std::vector<Math::Point> &&controlPoints);

        bool intersect(const Math::Ray &ray, Intersection &isect, bool closest) const override;
        BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

        void writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const;

    private:
        std::unique_ptr<Grid> mGrid;
    };
}

#endif
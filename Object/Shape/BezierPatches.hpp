#ifndef OBJECT_SHAPE_BEZIERPATCHES_HPP
#define OBJECT_SHAPE_BEZIERPATCHES_HPP

#include "Object/Shape/Base.hpp"
#include "Object/Shape/BezierPatch.hpp"

namespace Object {
    namespace Shape {
        class BezierPatches : public Base
        {
        public:
            BezierPatches(std::vector<std::unique_ptr<BezierPatch>> &&grids);

            bool intersect(const Math::Ray &ray, Intersection &isect) const override;
            BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

            void writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const override;

        private:
            std::vector<std::unique_ptr<BezierPatch>> mPatches;
            std::vector<BoundingVolume> mVolumes;
        };
    }
}

#endif
#ifndef OBJECT_SHAPE_TRANSFORMED_HPP
#define OBJECT_SHAPE_TRANSFORMED_HPP

#include "Object/Shape/Base.hpp"

#include "Math/Transformation.hpp"

#include <memory>

namespace Object::Shape {
    class Transformed : public Base
    {
    public:
        Transformed(std::unique_ptr<Base> shape, const Math::Transformation &transformation);

        bool intersect(const Math::Ray &ray, Intersection &isect, bool closest) const override;
        BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

        void writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const override;

    private:
        std::unique_ptr<Base> mShape;
        Math::Transformation mTransformation;
    };
}

#endif
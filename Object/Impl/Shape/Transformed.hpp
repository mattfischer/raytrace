#ifndef OBJECT_IMPL_SHAPE_TRANSFORMED_HPP
#define OBJECT_IMPL_SHAPE_TRANSFORMED_HPP

#include "Object/Shape.hpp"

#include "Math/Transformation.hpp"

#include <memory>

namespace Object::Impl::Shape {
    class Transformed : public Object::Shape
    {
    public:
        Transformed(std::unique_ptr<Object::Shape> shape, const Math::Transformation &transformation);

        bool intersect(const Math::Ray &ray, Intersection &isect, bool closest) const override;
        BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

        void writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const override;

    private:
        std::unique_ptr<Object::Shape> mShape;
        Math::Transformation mTransformation;
    };
}

#endif
#ifndef OBJECT_PRIMITIVE_HPP
#define OBJECT_PRIMITIVE_HPP

#include "Object/Surface.hpp"
#include "Object/BoundingVolume.hpp"
#include "Object/Shape/Base.hpp"
#include "Object/CLProxies.hpp"
#include "OpenCL.hpp"

namespace Object {
    class Primitive
    {
    public:
        Primitive(std::unique_ptr<Shape::Base> shape, std::unique_ptr<Surface> surface);

        const Shape::Base &shape() const;
        const Surface &surface() const;
        const BoundingVolume &boundingVolume() const;

        void writeProxy(PrimitiveProxy &proxy, OpenCL::Allocator &clAllocator) const;

    protected:
        std::unique_ptr<Shape::Base> mShape;
        std::unique_ptr<Surface> mSurface;
        BoundingVolume mBoundingVolume;
    };
}

#endif

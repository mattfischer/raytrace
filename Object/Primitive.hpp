#ifndef OBJECT_PRIMITIVE_HPP
#define OBJECT_PRIMITIVE_HPP

#include "Object/Surface.hpp"
#include "Object/BoundingVolume.hpp"
#include "Object/Light.hpp"
#include "Object/Shape.hpp"
#include "Object/CLProxies.hpp"
#include "OpenCL.hpp"

namespace Object {
    class Primitive
    {
    public:
        Primitive(std::unique_ptr<Object::Shape> shape, std::unique_ptr<Object::Surface> surface);

        const Object::Shape &shape() const;
        const Object::Surface &surface() const;
        const Object::BoundingVolume &boundingVolume() const;
        const std::unique_ptr<Object::Light> &light() const;

        void writeProxy(PrimitiveProxy &proxy, OpenCL::Allocator &clAllocator) const;

    protected:
        std::unique_ptr<Object::Shape> mShape;
        std::unique_ptr<Object::Surface> mSurface;
        std::unique_ptr<Object::Light> mLight;
        Object::BoundingVolume mBoundingVolume;
    };
}

#endif

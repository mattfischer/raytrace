#include "Object/Primitive.hpp"

namespace Object {
    Primitive::Primitive(std::unique_ptr<Object::Shape> shape, std::unique_ptr<Object::Surface> surface)
        : mShape(std::move(shape)), mSurface(std::move(surface))
    {
        mBoundingVolume = mShape->boundingVolume(Math::Transformation());
    }

    const Object::Shape &Primitive::shape() const
    {
        return *mShape;
    }

    const Object::Surface &Primitive::surface() const
    {
        return *mSurface;
    }

    const BoundingVolume &Primitive::boundingVolume() const
    {
        return mBoundingVolume;
    }

    void Primitive::writeProxy(PrimitiveProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        mShape->writeProxy(proxy.shape, clAllocator);
        mSurface->writeProxy(proxy.surface, clAllocator);
    }
}
#include "Object/Primitive.hpp"

#include "Object/Impl/Light/Shape.hpp"

namespace Object {
    Primitive::Primitive(std::unique_ptr<Object::Shape> shape, std::unique_ptr<Object::Surface> surface)
        : mShape(std::move(shape)), mSurface(std::move(surface))
    {
        mBoundingVolume = mShape->boundingVolume(Math::Transformation());
        if(mSurface->radiance().magnitude() > 0.0f) {
            mLight = std::make_unique<Object::Impl::Light::Shape>(*mShape, mSurface->radiance());
        }
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

    const std::unique_ptr<Object::Light> &Primitive::light() const
    {
        return mLight;
    }

    void Primitive::writeProxy(PrimitiveProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        mShape->writeProxy(proxy.shape, clAllocator);
        mSurface->writeProxy(proxy.surface, clAllocator);
    }
}
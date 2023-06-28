#include "Object/Primitive.hpp"

namespace Object {
    Primitive::Primitive(std::unique_ptr<Shape::Base> shape, std::unique_ptr<Surface> surface)
        : mShape(std::move(shape)), mSurface(std::move(surface))
    {
        mBoundingVolume = mShape->boundingVolume(Math::Transformation());
    }

    const Shape::Base &Primitive::shape() const
    {
        return *mShape;
    }

    const Surface &Primitive::surface() const
    {
        return *mSurface;
    }

    const BoundingVolume &Primitive::boundingVolume() const
    {
        return mBoundingVolume;
    }

    void Primitive::writeProxy(PrimitiveProxy &proxy) const
    {
        mShape->writeProxy(proxy.shape);
        mSurface->writeProxy(proxy.surface);
        proxy.primitive = (uintptr_t)this;
    }
}
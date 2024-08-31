#include "Object/Shape/Transformed.hpp"

namespace Object::Shape {
    Transformed::Transformed(std::unique_ptr<Base> shape, const Math::Transformation &transformation)
        : mShape(std::move(shape)), mTransformation(transformation)
    {
    }

    bool Transformed::intersect(const Math::Ray &ray, Intersection &isect, bool closest) const
    {
        Math::Ray transformedRay = mTransformation.inverse() * ray;
        if (mShape->intersect(transformedRay, isect, closest)) {
            isect.normal = (mTransformation * isect.normal).normalize();
            isect.tangent = mTransformation * isect.tangent;
            return true;
        }

        return false;
    }

    BoundingVolume Transformed::boundingVolume(const Math::Transformation &trans) const
    {
        return mShape->boundingVolume(trans * mTransformation);
    }

    void Transformed::writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        proxy.type = ShapeProxy::Type::Transformed;
        mTransformation.writeProxy(proxy.transformed.transformation);
        proxy.transformed.shape = clAllocator.allocate<ShapeProxy>();
        mShape->writeProxy(*proxy.transformed.shape, clAllocator);
    }
}
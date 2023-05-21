#include "Object/Shape/Transformed.hpp"

namespace Object {
    namespace Shape {
        Transformed::Transformed(std::unique_ptr<Base> shape, const Math::Transformation &transformation)
            : mShape(std::move(shape)), mTransformation(transformation)
        {
        }

        bool Transformed::intersect(const Math::Ray &ray, Intersection &isect) const
        {
            Math::Ray transformedRay = mTransformation.inverse() * ray;
            if (mShape->intersect(transformedRay, isect)) {
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
    }
}
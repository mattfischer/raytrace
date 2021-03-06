#include "Object/Shape/Quad.hpp"

#include <math.h>
#include <algorithm>

namespace Object {
    namespace Shape {
        Quad::Quad(const Math::Point &position, const Math::Vector &side1, const Math::Vector &side2)
            : mPosition(position), mSide1(side1), mSide2(side2)
        {
            mNormal = Math::Normal(mSide1 % mSide2).normalize();
            mTangent = Math::Bivector(mSide1, mSide2);
        }

        bool Quad::intersect(const Math::Ray &ray, Intersection &intersection) const
        {
            float distance = ((ray.origin() - mPosition) * mNormal) / (ray.direction() * -mNormal);
            if (distance >= 0 && distance < intersection.distance) {
                Math::Point point = ray.origin() + ray.direction() * distance;
                float u = (point - mPosition) * mSide1 / mSide1.magnitude2();
                float v = (point - mPosition) * mSide2 / mSide2.magnitude2();
                if (u >= 0 && u <= 1 && v >= 0 && v <= 1)
                {
                    intersection.distance = distance;
                    intersection.normal = mNormal;
                    intersection.tangent = mTangent;
                    intersection.surfacePoint = Math::Point2D(u, v);
                    return true;
                }
            }

            return false;
        }

        const Base::Sampler *Quad::sampler() const
        {
            return this;
        }

        float Quad::surfaceArea() const
        {
            return (mSide1 % mSide2).magnitude();
        }

        bool Quad::sample(const Math::Point2D &surfacePoint, Math::Point &point, Math::Normal &normal) const
        {
            point = mPosition + mSide1 * surfacePoint.u() + mSide2 * surfacePoint.v();
            normal = mNormal;

            return true;
        }

        BoundingVolume Quad::boundingVolume(const Math::Transformation &transformation) const
        {
            Math::Point points[] = { mPosition, mPosition + mSide1, mPosition + mSide2, mPosition + mSide1 + mSide2 };

            BoundingVolume volume;
            for (const Math::Point &point : points) {
                volume.expand(transformation * point);
            }

            return volume;
        }
    }
}
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

        bool Quad::intersect(const Math::Ray &ray, Intersection &isect) const
        {
            float distance = ((ray.origin() - mPosition) * mNormal) / (ray.direction() * -mNormal);
            if (distance >= 0 && distance < isect.distance) {
                Math::Point point = ray.origin() + ray.direction() * distance;
                float u = (point - mPosition) * mSide1 / mSide1.magnitude2();
                float v = (point - mPosition) * mSide2 / mSide2.magnitude2();
                if (u >= 0 && u <= 1 && v >= 0 && v <= 1)
                {
                    isect.distance = distance;
                    isect.normal = mNormal;
                    isect.tangent = mTangent;
                    isect.surfacePoint = Math::Point2D(u, v);
                    return true;
                }
            }

            return false;
        }

        bool Quad::sample(Math::Sampler::Base &sampler, Math::Point &pnt, Math::Normal &nrm, float &pdf) const
        {
            Math::Point2D pntSurface = sampler.getValue2D();
            pnt = mPosition + mSide1 * pntSurface.u() + mSide2 * pntSurface.v();
            nrm = mNormal;
            
            float surfaceArea = (mSide1 % mSide2).magnitude();
            pdf = 1.0f / surfaceArea;

            return true;
        }

        float Quad::samplePdf(const Math::Point &) const
        {
            float surfaceArea = (mSide1 % mSide2).magnitude();
            return 1.0f / surfaceArea;
        }

        BoundingVolume Quad::boundingVolume(const Math::Transformation &trans) const
        {
            Math::Point points[] = { mPosition, mPosition + mSide1, mPosition + mSide2, mPosition + mSide1 + mSide2 };

            BoundingVolume volume;
            for (const Math::Point &point : points) {
                volume.expand(trans * point);
            }

            return volume;
        }

        void Quad::writeProxy(ShapeProxy &proxy) const
        {
            proxy.type = ShapeProxy::Type::Quad;
            mPosition.writeProxy(proxy.quad.position);
            mSide1.writeProxy(proxy.quad.side1);
            mSide2.writeProxy(proxy.quad.side2);
            mNormal.writeProxy(proxy.quad.normal);
        }
    }
}
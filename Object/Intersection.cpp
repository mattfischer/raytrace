#include "Object/Intersection.hpp"
#include <cfloat>

namespace Object {
    Intersection::Intersection()
        : mScene(nullptr), mPrimitive(nullptr), mBeam(nullptr)
    {
        mShapeIntersection.distance = FLT_MAX;
        mNormalValid = false;
        mAlbedoValid = false;
        mSurfaceProjectionValid = false;
    }

    Intersection::Intersection(const Object::Scene &scene, const Object::Primitive &primitive, const Math::Beam &beam, const Object::Shape::Base::Intersection &shapeIntersection)
        : mScene(&scene), mPrimitive(&primitive), mBeam(&beam), mShapeIntersection(shapeIntersection)
    {
        mPoint = mBeam->ray().origin() + mBeam->ray().direction() * mShapeIntersection.distance;
        mNormalValid = false;
        mAlbedoValid = false;
        mSurfaceProjectionValid = false;
    };

    Intersection::Intersection(const IntersectionProxy &proxy)
    {
        mPoint = Math::Point(proxy.point);
    }

    const Math::Point &Intersection::point() const
    {
        return mPoint;
    }

    bool Intersection::valid() const
    {
        return mPrimitive != nullptr;
    }

    const Object::Scene &Intersection::scene() const
    {
        return *mScene;
    }

    const Object::Primitive &Intersection::primitive() const
    {
        return *mPrimitive;
    }

    const Math::Ray &Intersection::ray() const
    {
        return mBeam->ray();
    }

    const Math::Beam &Intersection::beam() const
    {
        return *mBeam;
    }

    float Intersection::distance() const
    {
        return mShapeIntersection.distance;
    }

    const Object::Shape::Base::Intersection &Intersection::shapeIntersection() const
    {
        return mShapeIntersection;
    }

    const Math::Normal &Intersection::normal() const
    {
        if (!mNormalValid) {
            mNormal = mShapeIntersection.normal;
            if(mPrimitive->surface().hasNormalMap()) {
                mNormal = mPrimitive->surface().normalMap().perturbNormal(mShapeIntersection.surfacePoint, surfaceProjection(), mShapeIntersection.normal, mShapeIntersection.tangent);
            }
            Math::Vector outgoingDirection = -mBeam->ray().direction();
            float dot = mNormal * outgoingDirection;
            mFacingNormal = (dot > 0) ? mNormal : -mNormal;
            mNormalValid = true;
        }

        return mNormal;
    }

    const Math::Normal &Intersection::facingNormal() const
    {
        normal();
        return mFacingNormal;
    }

    Object::Color Intersection::albedo() const
    {
        if (!mAlbedoValid) {
            Math::Bivector2D projection;
            if(mPrimitive->surface().albedo().needSurfaceProjection()) {
                projection = surfaceProjection();
            }
            mAlbedo = mPrimitive->surface().albedo().color(mShapeIntersection.surfacePoint, projection);
            mAlbedoValid = true;
        }

        return mAlbedo;
    }

    const Math::Bivector2D &Intersection::surfaceProjection() const
    {
        if (!mSurfaceProjectionValid) {
            Math::Bivector projection = mBeam->project(mShapeIntersection.distance, mShapeIntersection.normal);
            Math::Vector v = mShapeIntersection.tangent.u() % mShapeIntersection.tangent.v();
            v = v / v.magnitude2();
            Math::Vector2D du((projection.u() % mShapeIntersection.tangent.v()) * v, (mShapeIntersection.tangent.u() % projection.u()) * v);
            Math::Vector2D dv((projection.v() % mShapeIntersection.tangent.v()) * v, (mShapeIntersection.tangent.u() % projection.v()) * v);
            mSurfaceProjection = Math::Bivector2D(du, dv);
            mSurfaceProjectionValid = true;
        }

        return mSurfaceProjection;
    }

    void Intersection::writeProxy(IntersectionProxy &proxy) const
    {
        mPoint.writeProxy(proxy.point);
        mShapeIntersection.writeProxy(proxy.shapeIntersection);
    }
}

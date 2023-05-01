#include "Object/Intersection.hpp"
#include <cfloat>

namespace Object {
    Intersection::Intersection()
        : mScene(*(Object::Scene*)0), mPrimitive(*(Object::Primitive*)0), mBeam(*(Math::Beam*)0)
    {
        mShapeIntersection.distance = FLT_MAX;
        Object::Surface::initIntersectionCache(mSurfaceCache);
    }

    Intersection::Intersection(const Object::Scene &scene, const Object::Primitive &primitive, const Math::Beam &beam, const Object::Shape::Base::Intersection &shapeIntersection)
        : mScene(scene), mPrimitive(primitive), mBeam(beam), mShapeIntersection(shapeIntersection)
    {
        mPoint = mBeam.ray().origin() + mBeam.ray().direction() * mShapeIntersection.distance;
        Object::Surface::initIntersectionCache(mSurfaceCache);
    };

    const Math::Point &Intersection::point() const
    {
        return mPoint;
    }

    bool Intersection::valid() const
    {
        return mShapeIntersection.distance != FLT_MAX;
    }

    const Object::Scene &Intersection::scene() const
    {
        return mScene;
    }

    const Object::Primitive &Intersection::primitive() const
    {
        return mPrimitive;
    }

    const Math::Ray &Intersection::ray() const
    {
        return mBeam.ray();
    }

    const Math::Beam &Intersection::beam() const
    {
        return mBeam;
    }

    float Intersection::distance() const
    {
        return mShapeIntersection.distance;
    }

    const Object::Shape::Base::Intersection &Intersection::shapeIntersection() const
    {
        return mShapeIntersection;
    }

    Surface::IntersectionCache &Intersection::surfaceCache() const
    {
        return mSurfaceCache;
    }
}

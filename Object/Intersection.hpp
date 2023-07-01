#ifndef OBJECT_INTERSECTION_HPP
#define OBJECT_INTERSECTION_HPP

#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Transformation.hpp"
#include "Math/Beam.hpp"

#include "Object/Primitive.hpp"

#include "Object/Shape/Base.hpp"

#include <vector>

namespace Object {
    class Scene;
    class Intersection
    {
    public:
        Intersection();
        Intersection(const Object::Scene &scene, const Object::Primitive &primitive, const Math::Beam &beam, const Object::Shape::Base::Intersection &shapeIntersection);
        Intersection(const IntersectionProxy &proxy);

        bool valid() const;

        const Object::Scene &scene() const;
        const Object::Primitive &primitive() const;

        float distance() const;
        const Math::Ray &ray() const;
        const Math::Beam &beam() const;
        const Math::Point &point() const;
        const Object::Shape::Base::Intersection &shapeIntersection() const;

        const Math::Normal &normal() const;
        const Math::Normal &facingNormal() const;

        Object::Color albedo() const;

        const Math::Bivector2D &surfaceProjection() const;

        void writeProxy(IntersectionProxy &proxy) const;

    protected:
        const Object::Scene *mScene;
        const Object::Primitive *mPrimitive;
        const Math::Beam *mBeam;
        Object::Shape::Base::Intersection mShapeIntersection;
        Math::Point mPoint;

        mutable bool mNormalValid;
        mutable Math::Normal mNormal;
        mutable Math::Normal mFacingNormal;

        mutable bool mAlbedoValid;
        mutable Object::Color mAlbedo;

        mutable bool mSurfaceProjectionValid;
        mutable Math::Bivector2D mSurfaceProjection;
    };
}

#endif

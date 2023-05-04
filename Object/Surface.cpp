#include "Object/Surface.hpp"

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"
#include "Object/Intersection.hpp"

#include "Parse/AST.h"

namespace Object {
    void Surface::initIntersectionCache(IntersectionCache &intersectionCache)
    {
        intersectionCache.surfaceProjectionValid = false;
        intersectionCache.albedoValid = false;
        intersectionCache.normalValid = false;
    }

    Surface::Surface(std::unique_ptr<Albedo::Base> albedo, std::unique_ptr<Brdf::Base> brdf, const Object::Radiance &radiance, std::unique_ptr<Object::NormalMap> normalMap)
    {
        mAlbedo = std::move(albedo);
        mBrdf = std::move(brdf);
        mRadiance = radiance;
        mNormalMap = std::move(normalMap);
    }

    const Albedo::Base &Surface::albedo() const
    {
        return *mAlbedo;
    }

    const Brdf::Base &Surface::brdf() const
    {
        return *mBrdf;
    }

    const Object::Radiance &Surface::radiance() const
    {
        return mRadiance;
    }

    bool Surface::hasNormalMap() const
    {
        return mNormalMap.get();
    }

    const Object::NormalMap &Surface::normalMap() const
    {
        return *mNormalMap;
    }

    Object::Color Surface::reflected(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const
    {
        return mBrdf->reflected(incidentDirection, facingNormal(intersection), -intersection.ray().direction(), albedo(intersection));
    }

    Object::Color Surface::transmitted(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const
    {
        return mBrdf->transmitted(incidentDirection, -facingNormal(intersection), albedo(intersection));
    }

    Object::Color Surface::albedo(const Object::Intersection &intersection) const
    {
        IntersectionCache &intersectionCache = intersection.surfaceCache();

        if (!intersectionCache.albedoValid) {
            intersectionCache.albedo = mAlbedo->color(intersection.shapeIntersection().surfacePoint, surfaceProjection(intersection));
            intersectionCache.albedoValid = true;
        }

        return intersectionCache.albedo;
    }

    const Math::Bivector2D &Surface::surfaceProjection(const Object::Intersection &intersection) const
    {
        IntersectionCache &intersectionCache = intersection.surfaceCache();

        if (!intersectionCache.surfaceProjectionValid) {
            Math::Bivector projection = intersection.beam().project(intersection.shapeIntersection().distance, intersection.shapeIntersection().normal);
            Math::Vector v = intersection.shapeIntersection().tangent.u() % intersection.shapeIntersection().tangent.v();
            v = v / v.magnitude2();
            Math::Vector2D du((projection.u() % intersection.shapeIntersection().tangent.v()) * v, (intersection.shapeIntersection().tangent.u() % projection.u()) * v);
            Math::Vector2D dv((projection.v() % intersection.shapeIntersection().tangent.v()) * v, (intersection.shapeIntersection().tangent.u() % projection.v()) * v);
            intersectionCache.surfaceProjection = Math::Bivector2D(du, dv);
            intersectionCache.surfaceProjectionValid = true;
        }

        return intersectionCache.surfaceProjection;
    }

    const Math::Normal &Surface::normal(const Object::Intersection &intersection) const
    {
        IntersectionCache &intersectionCache = intersection.surfaceCache();

        if (!intersectionCache.normalValid) {
            intersectionCache.normal = intersection.shapeIntersection().normal;
            if(hasNormalMap()) {
                intersectionCache.normal = normalMap().perturbNormal(intersection.shapeIntersection().surfacePoint, surfaceProjection(intersection), intersection.shapeIntersection().normal, intersection.shapeIntersection().tangent);
            }
            Math::Vector outgoingDirection = -intersection.beam().ray().direction();
            float dot = intersectionCache.normal * outgoingDirection;
            intersectionCache.facingNormal = (dot > 0) ? intersectionCache.normal : -intersectionCache.normal;
            intersectionCache.normalValid = true;
        }

        return intersectionCache.normal;
    }

    const Math::Normal &Surface::facingNormal(const Object::Intersection &intersection) const
    {
        normal(intersection);
        return intersection.surfaceCache().facingNormal;
    }
}

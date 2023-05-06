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

    Surface::Surface(std::unique_ptr<Albedo::Base> albedo, std::vector<std::unique_ptr<Brdf::Base>> brdfs, float transmitIor, const Object::Radiance &radiance, std::unique_ptr<Object::NormalMap> normalMap)
    {
        mAlbedo = std::move(albedo);
        mBrdfs = std::move(brdfs);
        mTransmitIor = transmitIor;
        mRadiance = radiance;
        mNormalMap = std::move(normalMap);

        mLambert = 0;
        mOpaque = false;
        for(const std::unique_ptr<Brdf::Base> &brdf : mBrdfs) {
            if(brdf->opaque()) {
                mOpaque = true;
            }

            if(brdf->lambert() > 0) {
                mLambert = brdf->lambert();
            }
        }

        mTransmitIor = transmitIor;        
    }

    const Albedo::Base &Surface::albedo() const
    {
        return *mAlbedo;
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
        Object::Color color;
        Object::Color transmittedColor(1, 1, 1);

        for(const auto &brdf : mBrdfs) {
            color = color + transmittedColor * brdf->reflected(incidentDirection, facingNormal(intersection), -intersection.ray().direction(), albedo(intersection));
            transmittedColor = transmittedColor * brdf->transmitted(incidentDirection, facingNormal(intersection), albedo(intersection));
        }

        return color;
    }

    Object::Color Surface::transmitted(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const
    {
        Object::Color transmittedColor(1, 1, 1);

        for(const auto &brdf : mBrdfs) {
            transmittedColor = transmittedColor * brdf->transmitted(incidentDirection, -facingNormal(intersection), albedo(intersection));
        }

        return transmittedColor;
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

    Object::Color Surface::sample(const Object::Intersection &intersection, Render::Sampler &sampler, Math::Vector &incidentDirection, float &pdf, bool &pdfDelta) const
    {
        const Math::Vector outgoingDirection = -intersection.ray().direction();
        const Math::Normal &facingNormal = Surface::facingNormal(intersection);

        float transmitThreshold = opaque() ? 0 : 0.5f;
        if(transmitThreshold > 0) {
            float roulette = sampler.getValue();
            if(roulette < transmitThreshold) {
                pdfDelta = true;
                bool reverse = (Surface::normal(intersection) * outgoingDirection < 0);

                float ratio = 1.0f / mTransmitIor;
                if (reverse) {
                    ratio = 1.0f / ratio;
                }

                float c1 = outgoingDirection * facingNormal;
                float c2 = std::sqrt(1.0f - ratio * ratio * (1.0f - c1 * c1));

                incidentDirection = Math::Vector(facingNormal) * (ratio * c1 - c2) - outgoingDirection * ratio;
                pdf = 1.0f;

                return transmitted(intersection, incidentDirection) / (outgoingDirection * facingNormal * transmitThreshold);
            }
        }

        int idx = 0;
        if(mBrdfs.size() > 1) {
            float sample = sampler.getValue();
            idx = std::min((int)std::floor(mBrdfs.size() * sample), (int)mBrdfs.size() - 1);
        }
        const Object::Brdf::Base &brdf = *mBrdfs[idx];
       
        incidentDirection = brdf.sample(sampler, facingNormal, outgoingDirection);
        pdf = Surface::pdf(intersection, incidentDirection);
        pdfDelta = false;
        return reflected(intersection, incidentDirection) / (1 - transmitThreshold);
    }

    float Surface::pdf(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const
    {
        const Math::Vector outgoingDirection = -intersection.ray().direction();
        const Math::Normal &facingNormal = Surface::facingNormal(intersection);
        
        float totalPdf = 0;
        for(const std::unique_ptr<Object::Brdf::Base> &brdf : mBrdfs) {
            totalPdf += brdf->pdf(incidentDirection, facingNormal, outgoingDirection);
        }
        totalPdf /= (float)mBrdfs.size();

        return totalPdf;
    }

    bool Surface::opaque() const
    {
        return mOpaque;
    }

    float Surface::lambert() const
    {
        return mLambert;
    }

    float Surface::transmitIor() const
    {
        return mTransmitIor;
    }
}

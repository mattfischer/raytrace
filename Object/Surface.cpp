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

    Object::Color Surface::reflected(const Object::Intersection &isect, const Math::Vector &dirIn) const
    {
        Object::Color col;
        Object::Color colTransmit(1, 1, 1);

        for(const auto &brdf : mBrdfs) {
            col = col + colTransmit * brdf->reflected(dirIn, facingNormal(isect), -isect.ray().direction(), albedo(isect));
            colTransmit = colTransmit * brdf->transmitted(dirIn, facingNormal(isect), albedo(isect));
        }

        return col;
    }

    Object::Color Surface::transmitted(const Object::Intersection &isect, const Math::Vector &dirIn) const
    {
        Object::Color colTransmit(1, 1, 1);

        for(const auto &brdf : mBrdfs) {
            colTransmit = colTransmit * brdf->transmitted(dirIn, -facingNormal(isect), albedo(isect));
        }

        return colTransmit;
    }

    Object::Color Surface::albedo(const Object::Intersection &isect) const
    {
        IntersectionCache &isectCache = isect.surfaceCache();

        if (!isectCache.albedoValid) {
            isectCache.albedo = mAlbedo->color(isect.shapeIntersection().surfacePoint, surfaceProjection(isect));
            isectCache.albedoValid = true;
        }

        return isectCache.albedo;
    }

    const Math::Bivector2D &Surface::surfaceProjection(const Object::Intersection &isect) const
    {
        IntersectionCache &isectCache = isect.surfaceCache();

        if (!isectCache.surfaceProjectionValid) {
            Math::Bivector projection = isect.beam().project(isect.shapeIntersection().distance, isect.shapeIntersection().normal);
            Math::Vector v = isect.shapeIntersection().tangent.u() % isect.shapeIntersection().tangent.v();
            v = v / v.magnitude2();
            Math::Vector2D du((projection.u() % isect.shapeIntersection().tangent.v()) * v, (isect.shapeIntersection().tangent.u() % projection.u()) * v);
            Math::Vector2D dv((projection.v() % isect.shapeIntersection().tangent.v()) * v, (isect.shapeIntersection().tangent.u() % projection.v()) * v);
            isectCache.surfaceProjection = Math::Bivector2D(du, dv);
            isectCache.surfaceProjectionValid = true;
        }

        return isectCache.surfaceProjection;
    }

    const Math::Normal &Surface::normal(const Object::Intersection &isect) const
    {
        IntersectionCache &isectCache = isect.surfaceCache();

        if (!isectCache.normalValid) {
            isectCache.normal = isect.shapeIntersection().normal;
            if(hasNormalMap()) {
                isectCache.normal = normalMap().perturbNormal(isect.shapeIntersection().surfacePoint, surfaceProjection(isect), isect.shapeIntersection().normal, isect.shapeIntersection().tangent);
            }
            Math::Vector outgoingDirection = -isect.beam().ray().direction();
            float dot = isectCache.normal * outgoingDirection;
            isectCache.facingNormal = (dot > 0) ? isectCache.normal : -isectCache.normal;
            isectCache.normalValid = true;
        }

        return isectCache.normal;
    }

    const Math::Normal &Surface::facingNormal(const Object::Intersection &isect) const
    {
        normal(isect);
        return isect.surfaceCache().facingNormal;
    }

    Object::Color Surface::sample(const Object::Intersection &isect, Render::Sampler &sampler, Math::Vector &dirIn, float &pdf, bool &pdfDelta) const
    {
        const Math::Vector dirOut = -isect.ray().direction();
        const Math::Normal &nrmFacing = Surface::facingNormal(isect);

        float transmitThreshold = 0;
        if(!opaque()) {
            bool reverse = (Surface::normal(isect) * dirOut < 0);

            float ratio = 1.0f / mTransmitIor;
            if (reverse) {
                ratio = 1.0f / ratio;
            }

            float c1 = dirOut * nrmFacing;
            float c2 = std::sqrt(1.0f - ratio * ratio * (1.0f - c1 * c1));

            dirIn = Math::Vector(nrmFacing) * (ratio * c1 - c2) - dirOut * ratio;
            Object::Color throughput = transmitted(isect, -dirOut);
            transmitThreshold = std::min(1.0f, throughput.maximum());
            float roulette = sampler.getValue();

            if(roulette < transmitThreshold) {
                pdf = 1.0f;
                pdfDelta = true;

                return transmitted(isect, dirIn) / (dirOut * nrmFacing * transmitThreshold);
            }
        }

        int idx = 0;
        if(mBrdfs.size() > 1) {
            float sample = sampler.getValue();
            idx = std::min((int)std::floor(mBrdfs.size() * sample), (int)mBrdfs.size() - 1);
        }
        const Object::Brdf::Base &brdf = *mBrdfs[idx];
       
        dirIn = brdf.sample(sampler, nrmFacing, dirOut);
        pdf = Surface::pdf(isect, dirIn);
        pdfDelta = false;
        return reflected(isect, dirIn) / (1 - transmitThreshold);
    }

    float Surface::pdf(const Object::Intersection &isect, const Math::Vector &dirIn) const
    {
        const Math::Vector dirOut = -isect.ray().direction();
        const Math::Normal &nrmFacing = Surface::facingNormal(isect);
        
        float totalPdf = 0;
        for(const std::unique_ptr<Object::Brdf::Base> &brdf : mBrdfs) {
            totalPdf += brdf->pdf(dirIn, nrmFacing, dirOut);
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

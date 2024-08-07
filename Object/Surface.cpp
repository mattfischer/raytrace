#include "Object/Surface.hpp"

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"
#include "Object/Intersection.hpp"

#include "Parse/AST.h"

#include <cmath>

namespace Object {
    Surface::Surface(std::unique_ptr<Albedo::Base> albedo, std::vector<std::unique_ptr<Brdf::Base>> brdfs, float transmitIor, const Math::Radiance &radiance, std::unique_ptr<Object::NormalMap> normalMap)
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

    const Math::Radiance &Surface::radiance() const
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

    Math::Color Surface::reflected(const Object::Intersection &isect, const Math::Vector &dirIn) const
    {
        Math::Color col;
        Math::Color colTransmit(1, 1, 1);

        for(const auto &brdf : mBrdfs) {
            col = col + colTransmit * brdf->reflected(dirIn, isect.facingNormal(), -isect.ray().direction(), isect.albedo());
            colTransmit = colTransmit * brdf->transmitted(dirIn, isect.facingNormal(), isect.albedo());
        }

        return col;
    }

    Math::Color Surface::transmitted(const Object::Intersection &isect, const Math::Vector &dirIn) const
    {
        Math::Color colTransmit(1, 1, 1);

        for(const auto &brdf : mBrdfs) {
            colTransmit = colTransmit * brdf->transmitted(dirIn, -isect.facingNormal(), isect.albedo());
        }

        return colTransmit;
    }

    Math::Color Surface::sample(const Object::Intersection &isect, Math::Sampler::Base &sampler, Math::Vector &dirIn, float &pdf, bool &pdfDelta) const
    {
        const Math::Vector dirOut = -isect.ray().direction();
        const Math::Normal &nrmFacing = isect.facingNormal();

        float transmitThreshold = 0;
        if(!opaque()) {
            bool reverse = (isect.normal() * dirOut < 0);

            float ratio = 1.0f / mTransmitIor;
            if (reverse) {
                ratio = 1.0f / ratio;
            }

            float c1 = dirOut * nrmFacing;
            float c2 = std::sqrt(1.0f - ratio * ratio * (1.0f - c1 * c1));

            dirIn = Math::Vector(nrmFacing) * (ratio * c1 - c2) - dirOut * ratio;
            Math::Color throughput = transmitted(isect, -dirOut);
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
        const Math::Normal &nrmFacing = isect.facingNormal();
        
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

    void Surface::writeProxy(SurfaceProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        mRadiance.writeProxy(proxy.radiance);
        mAlbedo->writeProxy(proxy.albedo, clAllocator);
        proxy.numBrdfs = mBrdfs.size();
        proxy.brdfs = clAllocator.allocateArray<BrdfProxy>(proxy.numBrdfs);
        for(int i=0; i<mBrdfs.size(); i++) {
            mBrdfs[i]->writeProxy(proxy.brdfs[i]);
        }
        proxy.opaque = mOpaque;
        proxy.transmitIor = mTransmitIor;
        if(mNormalMap) {
            proxy.normalMap = clAllocator.allocate<NormalMapProxy>();
            mNormalMap->writeProxy(*proxy.normalMap, clAllocator);
        } else {
            proxy.normalMap = nullptr;
        }
    }

}

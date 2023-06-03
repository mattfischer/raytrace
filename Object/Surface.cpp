#include "Object/Surface.hpp"

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"
#include "Object/Intersection.hpp"

#include "Math/OrthonormalBasis.hpp"

#include "Parse/AST.h"

#include <cmath>

namespace Object {
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

        Math::Vector dirInLocal = isect.basis().worldToLocal(dirIn);
        Math::Vector dirOutLocal = isect.basis().worldToLocal(-isect.ray().direction());
    
        for(const auto &brdf : mBrdfs) {
            col = col + colTransmit * brdf->reflected(dirInLocal, dirOutLocal, isect.albedo());
            colTransmit = colTransmit * brdf->transmitted(dirInLocal, isect.albedo());
        }

        return col;
    }

    Object::Color Surface::transmitted(const Object::Intersection &isect, const Math::Vector &dirIn) const
    {
        Object::Color colTransmit(1, 1, 1);

        Math::Vector dirInLocal = isect.basis().worldToLocal(-dirIn);

        for(const auto &brdf : mBrdfs) {
            colTransmit = colTransmit * brdf->transmitted(dirInLocal, isect.albedo());
        }

        return colTransmit;
    }


    Object::Color Surface::sample(const Object::Intersection &isect, Math::Sampler::Base &sampler, Math::Vector &dirIn, float &pdf, bool &pdfDelta) const
    {
        const Math::Vector dirOut = -isect.ray().direction();
        Math::Vector dirOutLocal = isect.basis().worldToLocal(dirOut);
        Math::Vector dirInLocal;

        float transmitThreshold = 0;
        if(!opaque()) {
            bool reverse = (isect.normal() * dirOut < 0);

            float ratio = 1.0f / mTransmitIor;
            if (reverse) {
                ratio = 1.0f / ratio;
            }

            float c1 = dirOutLocal.z();
            float c2 = std::sqrt(1.0f - ratio * ratio * (1.0f - c1 * c1));

            dirInLocal = Math::Vector(0, 0, ratio * c1 - c2) - dirOutLocal * ratio;
            dirIn = isect.basis().localToWorld(dirInLocal);
            Object::Color throughput = transmitted(isect, -dirOut);
            transmitThreshold = std::min(1.0f, throughput.maximum());
            float roulette = sampler.getValue();

            if(roulette < transmitThreshold) {
                pdf = 1.0f;
                pdfDelta = true;

                return transmitted(isect, dirIn) / (dirOutLocal.z() * transmitThreshold);
            }
        }

        int idx = 0;
        if(mBrdfs.size() > 1) {
            float sample = sampler.getValue();
            idx = std::min((int)std::floor(mBrdfs.size() * sample), (int)mBrdfs.size() - 1);
        }
        const Object::Brdf::Base &brdf = *mBrdfs[idx];
       
        dirInLocal = brdf.sample(sampler, dirOutLocal);
        dirIn = isect.basis().localToWorld(dirInLocal);
        pdf = Surface::pdf(isect, dirIn);
        pdfDelta = false;
        return reflected(isect, dirIn) / (1 - transmitThreshold);
    }

    float Surface::pdf(const Object::Intersection &isect, const Math::Vector &dirIn) const
    {
        const Math::Vector dirOut = -isect.ray().direction();
        Math::Vector dirOutLocal = isect.basis().worldToLocal(dirOut);
        Math::Vector dirInLocal = isect.basis().worldToLocal(dirIn);

        float totalPdf = 0;
        for(const std::unique_ptr<Object::Brdf::Base> &brdf : mBrdfs) {
            totalPdf += brdf->pdf(dirInLocal, dirOutLocal);
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

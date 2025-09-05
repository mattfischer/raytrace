#ifndef OBJECT_IMPL_BRDF_OREN_NAYAR_HPP
#define OBJECT_IMPL_BRDF_OREN_NAYAR_HPP

#include "Object/Brdf.hpp"

namespace Object::Impl::Brdf {
    class OrenNayar : public Object::Brdf
    {
    public:
        OrenNayar(float strength, float roughness);

        Math::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const override;
        float lambert() const override;

        Math::Vector sample(Math::Sampler &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const override;
        Math::Pdf pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const override;

        void writeProxy(BrdfProxy &proxy) const override;
    
    private:
        float mStrength;
        float mRoughness;
    };
}

#endif

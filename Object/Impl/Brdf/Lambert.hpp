#ifndef OBJECT_IMPL_BRDF_LAMBERT_HPP
#define OBJECT_IMPL_BRDF_LAMBERT_HPP

#include "Object/Brdf.hpp"

namespace Object::Impl::Brdf {
    class Lambert : public Object::Brdf
    {
    public:
        Lambert(float strength);

        Math::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const override;
        float lambert() const override;

        Math::Vector sample(Math::Sampler &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const override;
        Math::Pdf pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const override;

        void writeProxy(BrdfProxy &proxy) const override;

    private:
        float mStrength;
    };
}

#endif

#ifndef OBJECT_IMPL_BRDF_PHONG_HPP
#define OBJECT_IMPL_BRDF_PHONG_HPP

#include "Object/Brdf.hpp"

namespace Object::Impl::Brdf {
    class Phong : public Object::Brdf
    {
    public:
        Phong(float strength, float power);

        Math::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const override;
        Math::Color transmitted(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Color &albedo) const override;

        Math::Vector sample(Math::Sampler &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const override;
        float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const override;

        void writeProxy(BrdfProxy &proxy) const override;

    private:
        float mStrength;
        float mPower;
    };
}

#endif

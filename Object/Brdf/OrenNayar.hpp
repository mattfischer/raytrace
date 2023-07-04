#ifndef OBJECT_BRDF_OREN_NAYAR_HPP
#define OBJECT_BRDF_OREN_NAYAR_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class OrenNayar : public Base
        {
        public:
            OrenNayar(float strength, float roughness);

            Math::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const override;
            float lambert() const override;

            Math::Vector sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const override;
            float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const override;

            void writeProxy(BrdfProxy &proxy) const override;
        
        private:
            float mStrength;
            float mRoughness;
        };
    }
}

#endif

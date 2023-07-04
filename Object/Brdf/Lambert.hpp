#ifndef OBJECT_BRDF_LAMBERT_HPP
#define OBJECT_BRDF_LAMBERT_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class Lambert : public Base
        {
        public:
            Lambert(float strength);

            Math::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const override;
            float lambert() const override;

            Math::Vector sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const override;
            float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const override;

            void writeProxy(BrdfProxy &proxy) const override;

        private:
            float mStrength;
        };
    }
}

#endif

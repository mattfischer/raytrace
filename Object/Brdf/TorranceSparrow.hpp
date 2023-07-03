#ifndef OBJECT_BRDF_TORRANCE_SPARROW_HPP
#define OBJECT_BRDF_TORRANCE_SPARROW_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class TorranceSparrow : public Base
        {
        public:
            TorranceSparrow(float strength, float roughness, float ior);

            Math::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const override;
            Math::Color transmitted(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Color &albedo) const override;

            Math::Vector sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const override;
            float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const override;
            bool opaque() const override;

        private:
            float mStrength;
            float mRoughness;
            float mIor;
        };
    }
}

#endif

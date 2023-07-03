#ifndef OBJECT_BRDF_PHONG_HPP
#define OBJECT_BRDF_PHONG_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class Phong : public Base
        {
        public:
            Phong(float strength, float power);

            Math::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const override;
            Math::Color transmitted(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Color &albedo) const override;

            Math::Vector sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const override;
            float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const override;

        private:
            float mStrength;
            float mPower;
        };
    }
}

#endif

#ifndef OBJECT_BRDF_OREN_NAYAR_HPP
#define OBJECT_BRDF_OREN_NAYAR_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class OrenNayar : public Base
        {
        public:
            OrenNayar(float strength, float roughness);

            Object::Color reflected(const Math::Vector &dirIn, const Math::Vector &dirOut, const Object::Color &albedo) const override;
            float lambert() const override;

            Math::Vector sample(Math::Sampler::Base &sampler, const Math::Vector &dirOut) const override;
            float pdf(const Math::Vector &dirIn, const Math::Vector &dirOut) const override;

        private:
            float mStrength;
            float mRoughness;
        };
    }
}

#endif

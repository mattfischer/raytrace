#ifndef OBJECT_BRDF_OREN_NAYAR_HPP
#define OBJECT_BRDF_OREN_NAYAR_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class OrenNayar : public Base
        {
        public:
            OrenNayar(float strength, float roughness);

            virtual Object::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Object::Color &albedo) const;
            virtual float lambert() const;

            Math::Vector sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const;
            float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const;

        private:
            float mStrength;
            float mRoughness;
        };
    }
}

#endif

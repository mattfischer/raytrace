#ifndef OBJECT_BRDF_TORRANCE_SPARROW_HPP
#define OBJECT_BRDF_TORRANCE_SPARROW_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class TorranceSparrow : public Base
        {
        public:
            TorranceSparrow(float strength, float roughness, float ior);

            virtual Object::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Object::Color &albedo) const;
            virtual Object::Color transmitted(const Math::Vector &dirIn, const Math::Normal &nrm, const Object::Color &albedo) const;

            virtual Math::Vector sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const;
            float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const;
            bool opaque() const;

        private:
            float mStrength;
            float mRoughness;
            float mIor;
        };
    }
}

#endif

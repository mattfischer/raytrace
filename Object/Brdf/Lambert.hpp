#ifndef OBJECT_BRDF_LAMBERT_HPP
#define OBJECT_BRDF_LAMBERT_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class Lambert : public Base
        {
        public:
            Lambert(float strength);

            virtual Object::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Object::Color &albedo) const;
            virtual float lambert() const;

            Math::Vector sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const;
            float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const;

        private:
            float mStrength;
        };
    }
}

#endif

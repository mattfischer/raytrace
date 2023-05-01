#ifndef OBJECT_BRDF_TORRANCE_SPARROW_HPP
#define OBJECT_BRDF_TORRANCE_SPARROW_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class TorranceSparrow : public Base
        {
        public:
            TorranceSparrow(float strength, float roughness, float ior);

            virtual Object::Color reflected(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
            virtual Object::Color transmitted(const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const;

            virtual Math::Vector sample(Render::Sampler &sampler, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;
            float pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;
            bool opaque() const;

        private:
            float mStrength;
            float mRoughness;
            float mIor;
        };
    }
}

#endif

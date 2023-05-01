#ifndef OBJECT_BRDF_PHONG_HPP
#define OBJECT_BRDF_PHONG_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class Phong : public Base
        {
        public:
            Phong(float strength, float power);

            virtual Object::Color reflected(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
            virtual Object::Color transmitted(const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const;

            virtual Math::Vector sample(Render::Sampler &sampler, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;
            float pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;

        private:
            float mStrength;
            float mPower;
        };
    }
}

#endif

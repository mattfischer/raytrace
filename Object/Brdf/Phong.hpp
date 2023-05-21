#ifndef OBJECT_BRDF_PHONG_HPP
#define OBJECT_BRDF_PHONG_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
    namespace Brdf {
        class Phong : public Base
        {
        public:
            Phong(float strength, float power);

            virtual Object::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Object::Color &albedo) const;
            virtual Object::Color transmitted(const Math::Vector &dirIn, const Math::Normal &nrm, const Object::Color &albedo) const;

            virtual Math::Vector sample(Render::Sampler &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const;
            float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const;

        private:
            float mStrength;
            float mPower;
        };
    }
}

#endif

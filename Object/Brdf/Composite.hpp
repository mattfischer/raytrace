#ifndef OBJECT_BRDF_COMPOSITE_HPP
#define OBJECT_BRDF_COMPOSITE_HPP

#include "Object/Brdf/Base.hpp"
#include "Render/Sampler.hpp"

#include <vector>

namespace Object {
    namespace Brdf {
        class Composite : public Base
        {
        public:
            Composite(std::vector<std::unique_ptr<Base>> brdfs, float transmitIor);

            virtual Object::Color reflected(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
            virtual Object::Color transmitted(const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const;

            virtual Math::Vector sample(Render::Sampler &sampler, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;
            virtual float pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;

            float lambert() const;

            bool opaque() const;
            float transmitIor() const;

        private:
            std::vector<std::unique_ptr<Base>> mBrdfs;
            float mTransmitIor;
            float mLambert;
            bool mOpaque;
        };
    }
}

#endif

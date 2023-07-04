#ifndef OBJECT_BRDF_BASE_HPP
#define OBJECT_BRDF_BASE_HPP

#include "Math/Radiance.hpp"
#include "Math/Color.hpp"

#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Point2D.hpp"

#include "Math/Sampler/Base.hpp"

#include "Object/Brdf/CLProxies.hpp"

#include <memory>

namespace Object {
    namespace Brdf {
        class Base {
        public:
            virtual ~Base();

            virtual Math::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const;
            virtual Math::Color transmitted(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Color &albedo) const;

            virtual float lambert() const;

            virtual Math::Vector sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const;
            virtual float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const;

            virtual bool opaque() const;
            virtual float transmitIor() const;

            virtual void writeProxy(BrdfProxy &proxy) const
            {
                proxy.type = BrdfProxy::Type::Lambert;
                proxy.lambert.strength = 1.0f;
            }
        };
    }
}

#endif

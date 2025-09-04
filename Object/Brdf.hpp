#ifndef OBJECT_BRDF_HPP
#define OBJECT_BRDF_HPP

#include "Math/Radiance.hpp"
#include "Math/Color.hpp"

#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Point2D.hpp"

#include "Math/Sampler.hpp"

#include "Object/Impl/Brdf/CLProxies.hpp"

#include <memory>

namespace Object {
    class Brdf {
    public:
        virtual ~Brdf();

        virtual Math::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const;
        virtual Math::Color transmitted(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Color &albedo) const;

        virtual float lambert() const;

        virtual Math::Vector sample(Math::Sampler &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const;
        virtual float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const;

        virtual bool opaque() const;
        virtual float transmitIor() const;

        virtual void writeProxy(BrdfProxy &proxy) const = 0;
    };
}

#endif

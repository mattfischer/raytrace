#ifndef OBJECT_BRDF_BASE_HPP
#define OBJECT_BRDF_BASE_HPP

#include "Object/Radiance.hpp"
#include "Object/Color.hpp"

#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Point2D.hpp"

#include "Math/Sampler/Base.hpp"

#include <memory>

namespace Object {
    namespace Brdf {
        class Base {
        public:
            virtual ~Base();

            virtual Object::Color reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Object::Color &albedo) const;
            virtual Object::Color transmitted(const Math::Vector &dirIn, const Math::Normal &nrm, const Object::Color &albedo) const;

            virtual float lambert() const;

            virtual Math::Vector sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const;
            virtual float pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const;

            virtual bool opaque() const;
            virtual float transmitIor() const;
        };
    }
}

#endif

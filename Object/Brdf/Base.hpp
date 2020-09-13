#ifndef OBJECT_BRDF_BASE_HPP
#define OBJECT_BRDF_BASE_HPP

#include "Object/Radiance.hpp"
#include "Object/Color.hpp"

#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Point2D.hpp"

#include "Render/Sampler.hpp"

#include <memory>

namespace Object {
	namespace Brdf {
		class Base {
		public:
            virtual Object::Radiance reflected(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
            virtual Object::Radiance transmitted(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const;

			virtual float lambert() const;

            virtual Math::Vector sample(Render::Sampler &sampler, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;
			virtual float pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;

            virtual bool opaque() const;
            virtual float transmitIor() const;
        };
	}
}

#endif

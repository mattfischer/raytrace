#ifndef OBJECT_BRDF_BASE_HPP
#define OBJECT_BRDF_BASE_HPP

#include "Object/Radiance.hpp"
#include "Object/Color.hpp"

#include "Math/Vector.hpp"
#include "Math/Normal.hpp"

#include <memory>

namespace Object {
	namespace Brdf {
		class Base {
		public:
			virtual Object::Radiance reflected(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
			virtual Object::Radiance transmitted(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const;

			virtual float lambert() const;

			virtual Math::Vector sample(float u, float v, const Math::Normal &normal, const Math::Vector &outgoingDirection, float &pdf) const;
		};
	}
}

#endif
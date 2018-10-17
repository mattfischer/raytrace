#include "Object/Brdf/Base.hpp"

#include "Parse/AST.h"

#include "Object/Brdf/Composite.hpp"

#include <vector>

namespace Object {
	namespace Brdf {
		Object::Radiance Base::radiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
		{
			return Object::Radiance(0, 0, 0);
		}

		float Base::lambert() const
		{
			return 0;
		}

		Math::Vector Base::sample(float u, float v, const Math::Normal &normal, const Math::Vector &outgoingDirection, float &pdf) const
		{
			pdf = 0;
			return Math::Vector(0, 0, 0);
		}
	}
}
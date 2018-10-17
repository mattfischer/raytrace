#define _USE_MATH_DEFINES
#include "Object/Brdf/Lambert.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include <cmath>

namespace Object {
	namespace Brdf {
		Lambert::Lambert(float strength)
		{
			mStrength = strength;
		}

		float Lambert::lambert() const
		{
			return mStrength;
		}

		Object::Radiance Lambert::radiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
		{
			return incidentRadiance * albedo * mStrength / M_PI;
		}
	}
}
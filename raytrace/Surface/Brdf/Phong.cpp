#include "Surface/Brdf/Phong.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include <cmath>

namespace Surface {
namespace Brdf {

Phong::Phong(float strength, float power)
{
	mStrength = strength;
	mPower = power;
}

Object::Color Phong::color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	Math::Vector incident = -incidentDirection;
	Math::Vector reflect = incident + Math::Vector(normal) * (2 * (-normal * incident));

	float dot = reflect * outgoingDirection;
	float coeff = 0;
	if(dot > 0) {
		coeff = std::pow(dot, mPower);
	}

	return incidentColor.scale(mStrength * coeff);
}

}
}
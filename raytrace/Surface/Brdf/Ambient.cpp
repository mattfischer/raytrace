#include "Surface/Brdf/Ambient.hpp"

namespace Surface {
namespace Brdf {

Ambient::Ambient(float strength)
{
	mStrength = strength;
}

Object::Color Ambient::color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	return incidentColor * albedo.scale(mStrength);
}

}
}
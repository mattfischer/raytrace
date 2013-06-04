#ifndef SURFACE_BRDF_SPECULAR_HPP
#define SURFACE_BRDF_SPECULAR_HPP

#include "Surface/Brdf/Base.hpp"

namespace Surface {
namespace Brdf {

class Specular : public Base
{
public:
	Specular(float strength, float power);

	virtual Object::Color color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;

private:
	float mStrength;
	float mPower;
};

}
}

#endif
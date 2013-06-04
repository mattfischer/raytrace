#ifndef SURFACE_BRDF_LAMBERT_HPP
#define SURFACE_BRDF_LAMBERT_HPP

#include "Surface/Brdf/Base.hpp"

namespace Surface {
namespace Brdf {

class Lambert : public Base
{
public:
	Lambert(float strength);

	virtual Object::Color color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;

private:
	float mStrength;
};

}
}

#endif
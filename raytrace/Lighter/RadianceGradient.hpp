#ifndef LIGHTER_RADIANCE_GRADIENT_HPP
#define LIGHTER_RADIANCE_GRADIENT_HPP

#include "Object/Radiance.hpp"
#include "Math/Vector.hpp"

namespace Lighter {

class RadianceGradient
{
public:
	RadianceGradient() = default;
	RadianceGradient(const Object::Radiance &radiance, const Math::Vector &vector);
	RadianceGradient(const Math::Vector &red, const Math::Vector &green, const Math::Vector &blue);

	Object::Radiance operator*(const Math::Vector &vector) const;
	RadianceGradient operator+(const RadianceGradient &other) const;
	RadianceGradient& operator+=(const RadianceGradient &other);
	RadianceGradient operator*(float other) const;
	RadianceGradient operator/(float other) const;

private:
	Math::Vector mRed;
	Math::Vector mGreen;
	Math::Vector mBlue;
};

}

#endif
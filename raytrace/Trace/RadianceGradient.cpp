#include "RadianceGradient.hpp"

namespace Trace {

RadianceGradient::RadianceGradient(const Object::Radiance &radiance, const Math::Vector &vector)
{
	mRed = vector * radiance.red();
	mGreen = vector * radiance.green();
	mBlue = vector * radiance.blue();
}

RadianceGradient::RadianceGradient(const Math::Vector &red, const Math::Vector &green, const Math::Vector &blue)
{
	mRed = red;
	mGreen = green;
	mBlue = blue;
}

Object::Radiance RadianceGradient::operator*(const Math::Vector &vector) const
{
	return Object::Radiance(mRed * vector, mGreen * vector, mBlue * vector);
}

RadianceGradient RadianceGradient::operator+(const RadianceGradient &other) const
{
	return RadianceGradient(mRed + other.mRed, mGreen + other.mGreen, mBlue + other.mBlue);
}

RadianceGradient &RadianceGradient::operator+=(const RadianceGradient &other)
{
	mRed = mRed + other.mRed;
	mGreen = mGreen + other.mGreen;
	mBlue = mBlue + other.mBlue;

	return *this;
}

RadianceGradient RadianceGradient::operator*(float other) const
{
	return RadianceGradient(mRed * other, mGreen * other, mBlue * other);
}

RadianceGradient RadianceGradient::operator/(float other) const
{
	return RadianceGradient(mRed / other, mGreen / other, mBlue / other);
}

}
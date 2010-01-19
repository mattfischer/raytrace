#include "light.h"

Light::Light(const Color &color)
: mColor(color)
{
}

Light::Light(const Light &c)
{
	mColor = c.mColor;
}

Light &Light::operator=(const Light &c)
{
	mColor = c.mColor;

	return *this;
}

const Color &Light::color() const
{
	return mColor;
}

void Light::setColor(const Color &color)
{
	mColor = color;
}

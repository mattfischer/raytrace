#include "Object/Color.hpp"

namespace Object {

Color::Color()
{
	mRed = 0;
	mGreen = 0;
	mBlue = 0;
}

Color::Color(float red, float green, float blue)
{
	mRed = red;
	mGreen = green;
	mBlue = blue;
}

Color::Color(const ASTVector &vector)
{
	mRed = vector.x;
	mGreen = vector.y;
	mBlue = vector.z;
}

Color::Color(const Color &c)
{
	mRed = c.mRed;
	mGreen = c.mGreen;
	mBlue = c.mBlue;
}

Color &Color::operator=(const Color &c)
{
	mRed = c.mRed;
	mGreen = c.mGreen;
	mBlue = c.mBlue;

	return *this;
}

float Color::red() const
{
	return mRed;
}

void Color::setRed(float red)
{
	mRed = red;
}

float Color::green() const
{
	return mGreen;
}

void Color::setGreen(float green)
{
	mGreen = green;
}

float Color::blue() const
{
	return mBlue;
}

void Color::setBlue(float blue)
{
	mBlue = blue;
}

Color Color::scale(float b) const
{
	return Color(mRed * b, mGreen * b, mBlue * b);
}

Color Color::clamp() const
{
	Color r(mRed, mGreen, mBlue);

	if(r.mRed < 0.0) r.mRed = 0;
	if(r.mRed > 1.0) r.mRed = 1;

	if(r.mGreen < 0.0) r.mGreen = 0;
	if(r.mGreen > 1.0) r.mGreen = 1;

	if(r.mBlue < 0.0) r.mBlue = 0;
	if(r.mBlue > 1.0) r.mBlue = 1;

	return r;
}

Color Color::operator+(const Color &b) const
{
	return Color(mRed + b.mRed, mGreen + b.mGreen, mBlue + b.mBlue);
}

Color &Color::operator+=(const Color &b)
{
	mRed += b.mRed;
	mGreen += b.mGreen;
	mBlue += b.mBlue;

	return *this;
}

Color Color::operator/(float b) const
{
	return Color(mRed / b, mGreen / b, mBlue / b);
}

Color Color::operator*(const Color &b) const
{
	return Color(mRed * b.mRed, mGreen * b.mGreen, mBlue * b.mBlue);
}

Color Color::operator*(float b) const
{
	return Color(mRed * b, mGreen * b, mBlue * b);
}

}
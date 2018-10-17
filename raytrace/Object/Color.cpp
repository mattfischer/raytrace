#include "Object/Color.hpp"

#include "Parse/AST.h"

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

	float Color::green() const
	{
		return mGreen;
	}

	float Color::blue() const
	{
		return mBlue;
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
#ifndef OBJECT_COLOR_HPP
#define OBJECT_COLOR_HPP

#include "Parse/Forwards.hpp"

namespace Object {

class Color
{
public:
	Color();
	Color(float red, float green, float blue);
	Color(const ASTVector &vector);
	Color(const Color &c);
	Color &operator=(const Color &c);

	float red() const;
	float green() const;
	float blue() const;

	Color scale(float b) const;
	Color clamp() const;

	Color operator+(const Color &b) const;
	Color &operator+=(const Color &b);
	Color operator/(float b) const;
	Color operator*(const Color &b) const;
	Color operator*(float b) const;

protected:
	float mRed;
	float mGreen;
	float mBlue;
};

}

#endif

#ifndef OBJECT_COLOR_HPP
#define OBJECT_COLOR_HPP

#include "Parse/AST.h"

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
	void setRed(float red);

	float green() const;
	void setGreen(float green);

	float blue() const;
	void setBlue(float blue);

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

#ifndef COLOR_H
#define COLOR_H

class Color
{
public:
	Color();
	Color(double red, double green, double blue);
	Color(const Color &c);
	Color &operator=(const Color &c);

	double red() const;
	void setRed(double red);

	double green() const;
	void setGreen(double green);

	double blue() const;
	void setBlue(double blue);

	Color scale(double b) const;
	Color clamp() const;

	Color operator+(const Color &b) const;
	Color &operator+=(const Color &b);
	Color operator/(double b) const;
	Color operator*(const Color &b) const;

protected:
	double mRed;
	double mGreen;
	double mBlue;
};

#endif

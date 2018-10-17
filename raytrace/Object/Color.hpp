#ifndef OBJECT_COLOR_HPP
#define OBJECT_COLOR_HPP

namespace Object {
	class Color
	{
	public:
		Color();
		Color(float red, float green, float blue);

		float red() const;
		float green() const;
		float blue() const;

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

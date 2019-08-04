#ifndef OBJECT_RADIANCE_HPP
#define OBJECT_RADIANCE_HPP

#include "Object/Color.hpp"

namespace Object {
	class Radiance
	{
	public:
		Radiance();
		Radiance(float red, float green, float blue);

		float red() const;
		float green() const;
		float blue() const;

		float magnitude() const;

		Radiance operator+(const Radiance &b) const;
		Radiance &operator+=(const Radiance &b);
		Radiance operator/(float b) const;
		Radiance operator*(const Radiance &b) const;
		Radiance operator*(float b) const;
		Radiance operator*(const Color &b) const;
		Radiance operator-(const Radiance &b) const;

		Radiance clamp() const;

	protected:
		float mRed;
		float mGreen;
		float mBlue;
	};
}

#endif

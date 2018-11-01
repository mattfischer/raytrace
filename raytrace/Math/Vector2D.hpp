#ifndef MATH_VECTOR_2D_HPP
#define MATH_VECTOR_2D_HPP

namespace Math {
	class Vector2D
	{
	public:
		Vector2D();
		Vector2D(float u, float v);

		float u() const;
		float v() const;

		Vector2D operator*(float b) const;
		Vector2D operator/(float b) const;
		Vector2D operator+(const Vector2D &b) const;
		Vector2D operator-(const Vector2D &b) const;

		float operator%(const Vector2D &b) const;

	private:
		float mU;
		float mV;
	};
}

#endif

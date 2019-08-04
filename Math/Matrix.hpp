#ifndef MATH_MATRIX_HPP
#define MATH_MATRIX_HPP

namespace Math {
	class Matrix
	{
	public:
		Matrix();
		Matrix(float m0, float m1, float m2, float m3, 
					   float m4, float m5, float m6, float m7,
					   float m8, float m9, float m10, float m11,
					   float m12, float m13, float m14, float m15);
		Matrix(const Matrix &c);
		Matrix &operator=(const Matrix &c);

		float &at(int x, int y);
		const float &at(int x, int y) const;
		float &operator()(int x, int y);
		const float &operator()(int x, int y) const;
		bool identity() const;

		Matrix partialTranspose() const;
		Matrix operator*(const Matrix &b) const;

	protected:
		float mValues[16];
		bool mIdentity;
	};
}

#endif

#include "Math/Matrix.hpp"

namespace Math {
	Matrix::Matrix()
	{
		mIdentity = true;
	}

	Matrix::Matrix(float m0, float m1, float m2, float m3,
				   float m4, float m5, float m6, float m7,
				   float m8, float m9, float m10, float m11,
				   float m12, float m13, float m14, float m15)
	{
		mValues[0] = m0; mValues[1] = m1; mValues[2] = m2; mValues[3] = m3;
		mValues[4] = m4; mValues[5] = m5; mValues[6] = m6; mValues[7] = m7;
		mValues[8] = m8; mValues[9] = m9; mValues[10] = m10; mValues[11] = m11;
		mValues[12] = m12; mValues[13] = m13; mValues[14] = m14; mValues[15] = m15;

		mIdentity = false;
	}

	Matrix::Matrix(const Matrix &c)
	{
		mIdentity = c.mIdentity;

		if (!mIdentity) {
			for (int i = 0; i < 16; i++)
			{
				mValues[i] = c.mValues[i];
			}
		}
	}

	Matrix &Matrix::operator=(const Matrix &c)
	{
		mIdentity = c.mIdentity;

		if(!mIdentity)
		{
			for(int i=0; i<16; i++)
			{
				mValues[i] = c.mValues[i];
			}
		}

		return *this;
	}

	float &Matrix::at(int x, int y)
	{
		return mValues[y * 4 + x];
	}

	const float &Matrix::at(int x, int y) const
	{
		return mValues[y * 4 + x];
	}

	float &Matrix::operator()(int x, int y)
	{
		return at(x, y);
	}

	const float &Matrix::operator()(int x, int y) const
	{
		return at(x, y);
	}

	bool Matrix::identity() const
	{
		return mIdentity;
	}

	Matrix Matrix::operator*(const Matrix &b) const
	{
		if(mIdentity) return b;
		if(b.mIdentity) return *this;

		Matrix r;

		for(int i=0; i<4; i++)
			for(int j=0; j<4; j++)
			{
				r(i, j) = 0;
				for(int k=0; k<4; k++)
				{
					r(i, j) += at(k, j) * b(i, k);
				}
			}

		r.mIdentity = false;

		return r;
	}

	Matrix Matrix::partialTranspose() const
	{
		return Matrix(at(0, 0), at(1, 0), at(2, 0), at(0, 3),
					  at(0, 1), at(1, 1), at(2, 1), at(1, 3),
					  at(0, 2), at(1, 2), at(2, 2), at(2, 3),
					  at(3, 0), at(3, 1), at(3, 2), at(3, 3));
	}
}
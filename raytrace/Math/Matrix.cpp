#include "Math/Matrix.hpp"

namespace Math {
Matrix::Matrix(double m0, double m1, double m2, double m3,
			   double m4, double m5, double m6, double m7,
			   double m8, double m9, double m10, double m11,
			   double m12, double m13, double m14, double m15)
{
	mValues[0] = m0; mValues[1] = m1; mValues[2] = m2; mValues[3] = m3;
	mValues[4] = m4; mValues[5] = m5; mValues[6] = m6; mValues[7] = m7;
	mValues[8] = m8; mValues[9] = m9; mValues[10] = m10; mValues[11] = m11;
	mValues[12] = m12; mValues[13] = m13; mValues[14] = m14; mValues[15] = m15;

	mIdentity = false;
}

Matrix::Matrix(const Matrix &c)
{
	for(int i=0; i<16; i++)
	{
		mValues[i] = c.mValues[i];
	}

	mIdentity = c.mIdentity;
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

Coordinate operator*(const Matrix &matrix, const Coordinate &coordinate)
{
	if(matrix.identity()) return coordinate;

	Coordinate r;

	for(int i=0; i<4; i++)
	{
		for(int k=0; k<4; k++)
			r(i) += matrix.at(k, i) * coordinate(k);
	}

	return r;
}

Coordinate operator*(const Coordinate &coordinate, const Matrix &matrix)
{
	if(matrix.identity()) return coordinate;

	Coordinate r;

	for(int i=0; i<4; i++)
	{
		for(int k=0; k<4; k++)
			r(i) += matrix.at(i, k) * coordinate(k);
	}

	return r;
}

}
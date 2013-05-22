#ifndef MATRIX_H
#define MATRIX_H

#include "Math/Point.hpp"
#include "Math/Vector.hpp"

namespace Math {

class Matrix
{
public:
	Matrix();
	Matrix(double m0, double m1, double m2, double m3, 
				   double m4, double m5, double m6, double m7,
				   double m8, double m9, double m10, double m11,
				   double m12, double m13, double m14, double m15);
	Matrix(const Matrix &c);
	Matrix &operator=(const Matrix &c);

	double &at(int x, int y);
	const double &at(int x, int y) const;
	double &operator()(int x, int y);
	const double &operator()(int x, int y) const;

	Point origin() const;
	Matrix transpose() const;

	Vector multiply(const Vector &b, bool useW) const;
	Matrix operator*(const Matrix &b) const;
	Vector operator*(const Vector &b) const;
	Point operator*(const Point &b) const;

protected:
	double mValues[16];
	bool mIdentity;
};

inline Matrix::Matrix()
{
	mIdentity = true;
}

inline Matrix::Matrix(double m0, double m1, double m2, double m3,
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

inline Matrix::Matrix(const Matrix &c)
{
	for(int i=0; i<16; i++)
	{
		mValues[i] = c.mValues[i];
	}

	mIdentity = c.mIdentity;
}

inline Matrix &Matrix::operator=(const Matrix &c)
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

inline double &Matrix::at(int x, int y)
{
	return mValues[y * 4 + x];
}

inline const double &Matrix::at(int x, int y) const
{
	return mValues[y * 4 + x];
}

inline double &Matrix::operator()(int x, int y)
{
	return at(x, y);
}

inline const double &Matrix::operator()(int x, int y) const
{
	return at(x, y);
}

inline Point Matrix::origin() const
{
	return Point(at(3,0), at(3,1), at(3,2));
}

inline Matrix Matrix::transpose() const
{
	if(mIdentity)
	{
		return *this;
	}

	Matrix r;

	r.mIdentity = false;

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			r.at(i, j) = at(j, i);

	return r;
}

inline Vector Matrix::multiply(const Vector &b, bool useW) const
{
	if(mIdentity) return b;

	Vector r;

	if(useW)
	{
		r = Vector(at(3,0), at(3,1), at(3,2));
	}

	for(int i=0; i<3; i++)
	{
		for(int k=0; k<3; k++)
			r(i) += at(k, i) * b(k);
	}

	return r;
}

inline Matrix Matrix::operator*(const Matrix &b) const
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

inline Vector Matrix::operator*(const Vector &b) const
{
	if(mIdentity) return b;

	Vector r;

	for(int i=0; i<3; i++)
	{
		for(int k=0; k<3; k++)
			r(i) += at(k, i) * b(k);
	}

	return r;
}

inline Point Matrix::operator*(const Point &b) const
{
	if(mIdentity) return b;

	Point r = Point(at(3,0), at(3,1), at(3,2));

	for(int i=0; i<3; i++)
	{
		for(int k=0; k<3; k++)
			r(i) += at(k, i) * b(k);
	}

	return r;
}

}

#endif

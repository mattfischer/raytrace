#ifndef MATRIX_H
#define MATRIX_H

#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Coordinate.hpp"

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
	bool identity() const;

	Matrix operator*(const Matrix &b) const;

protected:
	double mValues[16];
	bool mIdentity;
};

Coordinate operator*(const Matrix &matrix, const Coordinate &coordinate);
Coordinate operator*(const Coordinate &coordinate, const Matrix &matrix);

inline Matrix::Matrix()
{
	mIdentity = true;
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

inline bool Matrix::identity() const
{
	return mIdentity;
}

}

#endif

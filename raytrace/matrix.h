#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

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

	Matrix transpose() const;

	double &at(int x, int y);
	const double &at(int x, int y) const;

	Vector multiply(const Vector &b, bool useW) const;

	double &operator()(int x, int y);
	const double &operator()(int x, int y) const;

	Matrix operator*(const Matrix &b) const;
	Vector operator*(const Vector &b) const;

protected:
	double mValues[16];
	bool mIdentity;
};

#endif

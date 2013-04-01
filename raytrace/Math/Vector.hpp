#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

#include "ast.h"

namespace Math {

class Vector
{
public:
	Vector()
	{
		mValues[0] = 0;
		mValues[1] = 0;
		mValues[2] = 0;
	}

	Vector(double x, double y, double z)
	{
		mValues[0] = x;
		mValues[1] = y;
		mValues[2] = z;
	}

	Vector(const ASTVector &astVector)
	{
		mValues[0] = astVector.x;
		mValues[1] = astVector.y;
		mValues[2] = astVector.z;
	}

	Vector(const Vector &c)
	{
		for(int i=0; i<3; i++) 
			mValues[i] = c(i);
	}

	Vector &operator=(const Vector &c)
	{
		for(int i=0; i<3; i++) 
			mValues[i] = c(i);

		return *this;
	}

	double x() const
	{
		return mValues[0];
	}

	void setX(double x)
	{
		mValues[0] = x;
	}

	double y() const
	{
		return mValues[1];
	}

	void setY(double y)
	{
		mValues[1] = y;
	}

	double z() const
	{
		return mValues[2];
	}

	void setZ(double z)
	{
		mValues[2] = z;
	}

	Vector normalize() const
	{
		double m = magnitude();

		return Vector(x() / m, y() / m, z() / m);
	}

	double &at(int i)
	{
		return mValues[i];
	}

	const double &at(int i) const
	{
		return mValues[i];
	}

	double &operator()(int i)
	{
		return at(i);
	}

	const double &operator()(int i) const
	{
		return at(i);
	}

	double magnitude() const
	{
		return sqrt(magnitude2());
	}

	double magnitude2() const
	{
		return *this * *this;
	}

	Vector operator+(const Vector &b) const
	{
		return Vector(x() + b.x(), y() + b.y(), z() + b.z());
	}

	Vector operator-(const Vector &b) const
	{
		return Vector(x() - b.x(), y() - b.y(), z() - b.z());
	}

	double operator*(const Vector &b) const
	{
		return x() * b.x() + y() * b.y() + z() * b.z();
	}

	Vector operator*(double b) const
	{
		return Vector(x() * b, y() * b, z() * b);
	}

	Vector operator/(double b) const
	{
		return Vector(x() / b, y() / b, z() / b);
	}

	Vector operator-() const
	{
		return *this * -1;
	}

protected:
	double mValues[3];
};

}

#endif

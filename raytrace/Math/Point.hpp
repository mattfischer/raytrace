#ifndef POINT_H
#define POINT_H

#include <math.h>

#include "Math/Vector.hpp"
#include "Parse/AST.h"

namespace Math {

class Point
{
public:
	Point()
	{
		mValues[0] = 0;
		mValues[1] = 0;
		mValues[2] = 0;
	}

	Point(double x, double y, double z)
	{
		mValues[0] = x;
		mValues[1] = y;
		mValues[2] = z;
	}

	Point(const ASTVector &astVector)
	{
		mValues[0] = astVector.x;
		mValues[1] = astVector.y;
		mValues[2] = astVector.z;
	}

	Point(const Point &c)
	{
		for(int i=0; i<3; i++) 
			mValues[i] = c(i);
	}

	Point &operator=(const Point &c)
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

	Vector vector() const
	{
		return Vector(x(), y(), z());
	}

	Point operator+(const Vector &b) const
	{
		return Point(x() + b.x(), y() + b.y(), z() + b.z());
	}

	Point operator-(const Vector &b) const
	{
		return Point(x() - b.x(), y() - b.y(), z() - b.z());
	}

	Vector operator-(const Point &b) const
	{
		return Vector(x() - b.x(), y() - b.y(), z() - b.z());
	}

protected:
	double mValues[3];
};

}

#endif

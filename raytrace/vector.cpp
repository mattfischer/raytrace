#include "vector.h"

#include <math.h>

Vector::Vector()
{
	mValues[0] = 0;
	mValues[1] = 0;
	mValues[2] = 0;
}

Vector::Vector(double x, double y, double z)
{
	mValues[0] = x;
	mValues[1] = y;
	mValues[2] = z;
}

Vector::Vector(const Vector &c)
{
	for(int i=0; i<3; i++) 
		mValues[i] = c(i);
}

Vector &Vector::operator=(const Vector &c)
{
	for(int i=0; i<3; i++) 
		mValues[i] = c(i);

	return *this;
}

double Vector::x() const
{
	return mValues[0];
}

void Vector::setX(double x)
{
	mValues[0] = x;
}

double Vector::y() const
{
	return mValues[1];
}

void Vector::setY(double y)
{
	mValues[1] = y;
}

double Vector::z() const
{
	return mValues[2];
}

void Vector::setZ(double z)
{
	mValues[2] = z;
}

Vector Vector::normalize() const
{
	double m = magnitude();

	return Vector(x() / m, y() / m, z() / m);
}

double &Vector::at(int i)
{
	return mValues[i];
}

const double &Vector::at(int i) const
{
	return mValues[i];
}

double &Vector::operator()(int i)
{
	return at(i);
}

const double &Vector::operator()(int i) const
{
	return at(i);
}

double Vector::magnitude() const
{
	return sqrt(magnitude2());
}

double Vector::magnitude2() const
{
	return *this * *this;
}

Vector Vector::operator+(const Vector &b) const
{
	return Vector(x() + b.x(), y() + b.y(), z() + b.z());
}

Vector Vector::operator-(const Vector &b) const
{
	return Vector(x() - b.x(), y() - b.y(), z() - b.z());
}

double Vector::operator*(const Vector &b) const
{
	return x() * b.x() + y() * b.y() + z() * b.z();
}

Vector Vector::operator*(double b) const
{
	return Vector(x() * b, y() * b, z() * b);
}

Vector Vector::operator/(double b) const
{
	return Vector(x() / b, y() / b, z() / b);
}

Vector Vector::operator-() const
{
	return *this * -1;
}


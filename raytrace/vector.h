#ifndef VECTOR_H
#define VECTOR_H

#include "ast.h"

class Vector
{
public:
	Vector();
	Vector(double x, double y, double z);
	Vector(const ASTVector &astVector);
	Vector(const Vector &c);
	Vector &operator=(const Vector &c);

	double x() const;
	void setX(double x);

	double y() const;
	void setY(double y);

	double z() const;
	void setZ(double z);

	double &at(int i);
	const double &at(int i) const;

	double &operator()(int i);
	const double &operator()(int i) const;

	double magnitude() const;
	double magnitude2() const;
	Vector normalize() const;

	Vector operator+(const Vector &b) const;
	Vector operator-(const Vector &b) const;
	double operator*(const Vector &b) const;
	Vector operator*(double b) const;
	Vector operator/(double b) const;
	Vector operator-() const;
	
protected:
	double mValues[3];
};

#endif

#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

#include "Math/Coordinate.hpp"
#include "Parse/AST.h"

namespace Math {

class Point;
class Normal;
class BaseTransformation;
class Vector : public Coordinate
{
public:
	Vector();
	Vector(const Vector &c);
	Vector(float x, float y, float z);
	explicit Vector(const ASTVector &astVector);
	explicit Vector(const Point &point);
	explicit Vector(const Normal &normal);
	explicit Vector(const Coordinate &c);

	Vector normalize() const;
	float magnitude() const;
	float magnitude2() const;

	Vector operator+(const Vector &b) const;
	Vector operator-(const Vector &b) const;
	float operator*(const Vector &b) const;
	Vector operator*(float b) const;
	Vector operator/(float b) const;
	Vector operator-() const;
};

Vector operator*(const BaseTransformation &transformation, const Vector &vector);

}

#endif

#ifndef NORMAL_H
#define NORMAL_H

#include <math.h>

#include "Math/Coordinate.hpp"
#include "Parse/AST.h"

namespace Math {

class Point;
class Transformation;
class Vector;
class Normal : public Coordinate
{
public:
	Normal();
	Normal(const Normal &c);
	Normal(double x, double y, double z);
	explicit Normal(const ASTVector &astVector);
	explicit Normal(const Point &point);
	explicit Normal(const Coordinate &c);

	double operator*(const Normal &b) const;
	Normal operator-() const;
};

Normal operator*(const Transformation &transformation, const Normal &normal);
double operator*(const Vector &vector, const Normal &normal);
double operator*(const Normal &normal, const Vector &vector);

inline Normal::Normal()
{}

inline Normal::Normal(double x, double y, double z)
: Coordinate(x, y, z, 0)
{
}

inline Normal::Normal(const ASTVector &astVector)
: Coordinate(astVector, 0)
{
}

inline Normal::Normal(const Normal &c)
: Coordinate(c)
{
}

inline Normal::Normal(const Coordinate &c)
: Coordinate(c)
{
}

}

#endif

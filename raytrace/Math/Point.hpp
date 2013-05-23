#ifndef POINT_H
#define POINT_H

#include <math.h>

#include "Math/Coordinate.hpp"
#include "Parse/AST.h"

namespace Math {

class Vector;
class Transformation;
class Point : public Coordinate
{
public:
	Point();
	Point(float x, float y, float z);
	Point(const Point &c);
	explicit Point(const ASTVector &astVector);
	explicit Point(const Coordinate &c);

	Point operator+(const Vector &b) const;
	Point operator-(const Vector &b) const;
	Vector operator-(const Point &b) const;
};

Point operator*(const Transformation &transformation, const Point &point);

inline Point::Point()
{}

inline Point::Point(float x, float y, float z)
: Coordinate(x, y, z, 1)
{
}

inline Point::Point(const ASTVector &astVector)
: Coordinate(astVector, 1)
{
}

inline Point::Point(const Point &c)
: Coordinate(c)
{
}

inline Point::Point(const Coordinate &c)
: Coordinate(c)
{
}

}

#endif

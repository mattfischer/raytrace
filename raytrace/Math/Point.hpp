#ifndef POINT_H
#define POINT_H

#include <math.h>

#include "Math/Coordinate.hpp"
#include "Math/Vector.hpp"
#include "Parse/AST.h"

namespace Math {

class Point : public Coordinate
{
public:
	Point();
	Point(double x, double y, double z);
	Point(const Point &c);
	explicit Point(const ASTVector &astVector);

	Point operator+(const Vector &b) const;
	Point operator-(const Vector &b) const;
	Vector operator-(const Point &b) const;
};

inline Point::Point()
{}

inline Point::Point(double x, double y, double z)
: Coordinate(x, y, z)
{
}

inline Point::Point(const ASTVector &astVector)
: Coordinate(astVector)
{
}

inline Point::Point(const Point &c)
: Coordinate(c)
{
}

}

#endif

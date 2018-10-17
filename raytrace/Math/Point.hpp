#ifndef MATH_POINT_HPP
#define MATH_POINT_HPP

#include "Math/Coordinate.hpp"

#include "Parse/AST.h"

namespace Math {
	class Vector;
	class BaseTransformation;
	class Point : public Coordinate
	{
	public:
		Point();
		Point(float x, float y, float z);
		Point(const Point &c);
		explicit Point(const ASTVector &astVector);
		explicit Point(const Vector &vector);
		explicit Point(const Coordinate &c);

		Point operator+(const Vector &b) const;
		Point operator-(const Vector &b) const;
		Vector operator-(const Point &b) const;
	};

	Point operator*(const BaseTransformation &transformation, const Point &point);
}

#endif

#ifndef MATH_NORMAL_HPP
#define MATH_NORMAL_HPP

#include "Math/Forwards.hpp"
#include "Math/Coordinate.hpp"

#include "Parse/AST.h"

namespace Math {

class Point;
class BaseTransformation;
class Vector;
class Normal : public Coordinate
{
public:
	Normal();
	Normal(const Normal &c);
	Normal(float x, float y, float z);
	explicit Normal(const ASTVector &astVector);
	explicit Normal(const Point &point);
	explicit Normal(const Coordinate &c);

	Normal normalize() const;
	float magnitude() const;
	float magnitude2() const;

	float operator*(const Normal &b) const;
	float operator*(const Vector &b) const;
	Normal operator-() const;
};

Normal operator*(const BaseTransformation &transformation, const Normal &normal);
float operator*(const Vector &vector, const Normal &normal);

}

#endif

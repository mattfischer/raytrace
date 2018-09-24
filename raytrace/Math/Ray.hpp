#ifndef MATH_RAY_HPP
#define MATH_RAY_HPP

#include "Math/Point.hpp"
#include "Math/Vector.hpp"

namespace Math {

class Ray
{
public:
	static Ray createFromPoints(const Point &point1, const Point &point2, int generation);

	Ray();
	Ray(const Point &origin, const Vector &direction, int generation);
	Ray(const Ray &c);
	Ray &operator=(const Ray &c);

	const Point &origin() const;
	const Vector &direction() const;
	int generation() const;

protected:
	Point mOrigin;
	Vector mDirection;
	int mGeneration;
};

Ray operator*(const BaseTransformation &transformation, const Ray &ray);

}

#endif

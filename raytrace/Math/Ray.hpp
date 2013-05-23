#ifndef RAY_H
#define RAY_H

#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

namespace Math {

class Transformation;
class Ray
{
public:
	static Ray createFromPoints(const Point &point1, const Point &point2);

	Ray();
	Ray(const Point &origin, const Vector &direction);
	Ray(const Ray &c);
	Ray &operator=(const Ray &c);

	const Point &origin() const;
	void setOrigin(const Point &origin);

	const Vector &direction() const;
	void setDirection(const Vector &direction);

	int generation() const;
	void setGeneration(int generation);

protected:
	Point mOrigin;
	Vector mDirection;
	int mGeneration;
};

Ray operator*(const Transformation &transformation, const Ray &ray);

inline const Point &Ray::origin() const
{
	return mOrigin;
}

inline const Vector &Ray::direction() const
{
	return mDirection;
}

}

#endif

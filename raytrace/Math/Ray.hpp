#ifndef RAY_H
#define RAY_H

#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

namespace Math {

class Ray
{
public:
	static Ray createFromPoints(const Point &point1, const Point &point2);

	Ray();
	Ray(const Point &origin, const Vector &direction);
	Ray(const Ray &c);
	Ray &operator=(const Ray &c);

	const Point &origin() const { return mOrigin; }
	void setOrigin(const Point &origin);

	const Vector &direction() const { return mDirection; }
	void setDirection(const Vector &direction);

	int generation() const;
	void setGeneration(int generation);

	Ray transform(const Matrix &transformation) const;

protected:
	Point mOrigin;
	Vector mDirection;
	int mGeneration;
};

}

#endif

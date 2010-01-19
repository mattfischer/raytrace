#ifndef RAY_H
#define RAY_H

#include "vector.h"
#include "matrix.h"

class Ray
{
public:
	static Ray createFromPoints(const Vector &point1, const Vector &point2);

	Ray();
	Ray(const Vector &origin, const Vector &direction);
	Ray(const Ray &c);
	Ray &operator=(const Ray &c);

	const Vector &origin() const;
	void setOrigin(const Vector &origin);

	const Vector &direction() const;
	void setDirection(const Vector &direction);

	Ray transform(const Matrix &transformation) const;

protected:
 	Vector mOrigin;
	Vector mDirection;
};

#endif

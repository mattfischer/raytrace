#ifndef TRACE_RAY_HPP
#define TRACE_RAY_HPP

#include "Math/Point.hpp"
#include "Math/Vector.hpp"

namespace Trace {

class Ray
{
public:
	static Ray createFromPoints(const Math::Point &point1, const Math::Point &point2, int generation);

	Ray();
	Ray(const Math::Point &origin, const Math::Vector &direction, int generation);
	Ray(const Ray &c);
	Ray &operator=(const Ray &c);

	const Math::Point &origin() const;
	const Math::Vector &direction() const;
	int generation() const;

protected:
	Math::Point mOrigin;
	Math::Vector mDirection;
	int mGeneration;
};

Ray operator*(const Math::BaseTransformation &transformation, const Ray &ray);

}

#endif

#ifndef TRACE_RAY_HPP
#define TRACE_RAY_HPP

#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"

namespace Trace {

class Ray
{
public:
	static Ray createFromPoints(const Math::Point &point1, const Math::Point &point2);

	Ray();
	Ray(const Math::Point &origin, const Math::Vector &direction);
	Ray(const Ray &c);
	Ray &operator=(const Ray &c);

	const Math::Point &origin() const;
	void setOrigin(const Math::Point &origin);

	const Math::Vector &direction() const;
	void setDirection(const Math::Vector &direction);

	int generation() const;
	void setGeneration(int generation);

protected:
	Math::Point mOrigin;
	Math::Vector mDirection;
	int mGeneration;
};

Ray operator*(const Math::BaseTransformation &transformation, const Ray &ray);

}

#endif

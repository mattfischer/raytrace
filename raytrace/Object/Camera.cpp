#include "Object/Camera.hpp"

#include <math.h>
#include <stdio.h>

namespace Object {
	Camera::Camera(const Math::Point &position, const Math::Vector &direction, const Math::Vector &vertical, float fov)
	: mPosition(position), mDirection(direction)
	{
		float size = tan(fov * 3.14 / (2 * 180));
		Math::Vector perpVertical = (vertical - mDirection * (vertical * mDirection)).normalize();
		Math::Vector horizontal = (perpVertical % mDirection);

		mVertical = perpVertical * size;
		mHorizontal = horizontal * size;
	}

	Math::Ray Camera::createRay(float x, float y, Math::Bivector &differential) const
	{
		Math::Vector direction = (mDirection + mHorizontal * x + mVertical * y);
		float length = direction.magnitude();
		direction = direction / length;

		differential = Math::Bivector(mHorizontal, mVertical) / length;
		return Math::Ray(mPosition, direction);
	}

	float Camera::projectSize(float size, float distance) const
	{
		return size * mHorizontal.magnitude() * distance;
	}
}
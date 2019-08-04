#define _USE_MATH_DEFINES
#include "Object/Camera.hpp"

#include <math.h>
#include <stdio.h>

namespace Object {
	Camera::Camera(const Math::Point &position, const Math::Vector &direction, const Math::Vector &vertical, float fov, float focalLength, float apertureSize)
	: mPosition(position), mDirection(direction)
	{
		mImageSize = tan(fov * 3.14 / (2 * 180));
		Math::Vector perpVertical = (vertical - mDirection * (vertical * mDirection)).normalize();
		Math::Vector horizontal = (perpVertical % mDirection);
		mImagePlane = Math::Bivector(horizontal, perpVertical);
		mFocalLength = focalLength;
		mApertureSize = apertureSize;
	}

	Math::Ray Camera::createRay(const Math::Point2D &imagePoint, const Math::Point2D &aperturePoint, Math::Bivector &differential) const
	{
		Math::Vector direction = (mDirection + mImagePlane * Math::Vector2D(imagePoint) * mImageSize);
		float length = direction.magnitude();
		direction = direction / length;

		Math::Point p = mPosition + direction * mFocalLength;
		float r = std::sqrt(aperturePoint.u());
		float phi = 2 * M_PI * aperturePoint.v();
		Math::Vector2D apertureDiscPoint(r * std::cos(phi), r * std::sin(phi));
		Math::Point q = mPosition + mImagePlane * apertureDiscPoint * mApertureSize;
		direction = (p - q).normalize();

		differential = mImagePlane / length;
		return Math::Ray(q, direction);
	}

	Math::Beam Camera::createPixelBeam(const Math::Point2D &imagePoint, int width, int height, const Math::Point2D &aperturePoint) const
	{
		float cx = (2 * imagePoint.u() - width) / width;
		float cy = (2 * imagePoint.v() - height) / width;
		Math::Bivector dv;
		Math::Point2D imagePointTransformed(cx, -cy);
		Math::Ray ray = createRay(imagePointTransformed, aperturePoint, dv);

		float pixelSize = 2.0f / width;

		return Math::Beam(ray, Math::Bivector(), dv * pixelSize);
	}

	float Camera::projectSize(float size, float distance) const
	{
		return size * mImageSize * distance;
	}
}
#ifndef OBJECT_CAMERA_HPP
#define OBJECT_CAMERA_HPP

#include "Math/Ray.hpp"
#include "Math/Beam.hpp"
#include "Math/Bivector.hpp"
#include "Math/Point2D.hpp"

#include <memory>

namespace Object {
	class Camera
	{
	public:
		Camera(const Math::Point &position, const Math::Vector &direction, const Math::Vector &vertical, float fov, float focalLength, float apertureSize);

		Math::Ray createRay(const Math::Point2D &imagePoint, const Math::Point2D &aperturePoint, Math::Bivector &differential) const;
		Math::Beam createPixelBeam(const Math::Point2D &imagePoint, int width, int height, const Math::Point2D &aperturePoint) const;
		float projectSize(float size, float distance) const;

	private:
		Math::Point mPosition;
		Math::Vector mDirection;
		Math::Bivector mImagePlane;
		float mImageSize;
		float mFocalLength;
		float mApertureSize;
	};
}

#endif

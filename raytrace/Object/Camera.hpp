#ifndef OBJECT_CAMERA_HPP
#define OBJECT_CAMERA_HPP

#include "Math/Ray.hpp"
#include "Math/Bivector.hpp"

#include <memory>

namespace Object {
	class Camera
	{
	public:
		Camera(const Math::Point &position, const Math::Vector &direction, const Math::Vector &vertical, float fov);

		Math::Ray createRay(float x, float y, Math::Bivector &differential) const;
		float projectSize(float size, float distance) const;

	private:
		Math::Point mPosition;
		Math::Vector mDirection;
		Math::Vector mHorizontal;
		Math::Vector mVertical;
	};
}

#endif

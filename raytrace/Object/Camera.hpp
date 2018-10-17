#ifndef OBJECT_CAMERA_HPP
#define OBJECT_CAMERA_HPP

#include "Math/Ray.hpp"

#include <memory>

namespace Object {
	class Camera
	{
	public:
		Camera(const Math::Point &position, const Math::Vector &direction, const Math::Vector &vertical, float fov);

		static std::unique_ptr<Camera> fromAst(AST *ast);

		Math::Ray createRay(float x, float y) const;
		float projectSize(float size, float distance) const;

	private:
		Math::Point mPosition;
		Math::Vector mDirection;
		Math::Vector mHorizontal;
		Math::Vector mVertical;
	};
}

#endif

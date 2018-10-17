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

	std::unique_ptr<Camera> Camera::fromAst(AST *ast)
	{
		Math::Point position(ast->children[0]->data._vector);
		Math::Point lookAt(ast->children[1]->data._vector);

		std::unique_ptr<Camera> camera = std::make_unique<Camera>(position, (lookAt - position).normalize(), Math::Vector(0, 1, 0), 60.0f);

		return camera;
	}

	Math::Ray Camera::createRay(float x, float y) const
	{
		Math::Vector direction = (mDirection + mHorizontal * x + mVertical * y).normalize();

		return Math::Ray(mPosition, direction);
	}

	float Camera::projectSize(float size, float distance) const
	{
		return size * mHorizontal.magnitude() * distance;
	}
}
#include "Object/Camera.hpp"

#include <math.h>
#include <stdio.h>

namespace Object {

Camera::Camera(int fov)
{
	mSize = tan(fov * 3.14 / (2 * 180));
}

std::unique_ptr<Camera> Camera::fromAst(AST *ast)
{
	std::unique_ptr<Camera> camera = std::make_unique<Camera>(60);

	for(int i=0; i<ast->numChildren; i++)
	{
		switch(ast->children[i]->type)
		{
		case AstTransform:
			camera->transform(Math::Transformation::fromAst(ast->children[i]));
			break;
		}
	}

	return camera;
}

Math::Ray Camera::createRay(float x, float y) const
{
	float rayX, rayY;

	rayX = mSize * x;
	rayY = -mSize * y;

	return Math::Ray(transformation().origin(), transformation() * Math::Vector(rayX, rayY, 1).normalize());
}

float Camera::projectSize(float size, float distance) const
{
	return size * mSize * distance;
}

}
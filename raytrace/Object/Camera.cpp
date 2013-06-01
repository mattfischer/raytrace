#include "Object/Camera.hpp"

#include <math.h>
#include <stdio.h>

namespace Object {

static float rad(float deg)
{
	return deg * 3.14 / 180.0;
}

Camera::Camera(int hFOV)
{
	mWidth = tan(rad(hFOV/2));
}

Camera *Camera::fromAst(AST *ast)
{
	Camera *camera = new Camera(45);

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

Trace::Ray Camera::createRay(float x, float y, float aspectRatio, int generation)
{
	float rayX, rayY;

	rayX = mWidth * (2*x - 1);
	rayY = -mWidth * aspectRatio * (2*y - 1);

	return Trace::Ray(transformation().origin(), transformation() * Math::Vector(rayX, rayY, 1).normalize(), generation);
}

}
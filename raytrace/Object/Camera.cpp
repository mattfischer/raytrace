#include "Object/Camera.hpp"

#include <math.h>
#include <stdio.h>

namespace Object {

static double rad(double deg)
{
	return deg * 3.14 / 180.0;
}

#define DEFAULT_FOV 45
#define DEFAULT_ASPECT_RATIO 

Camera::Camera()
{
}

Camera *Camera::fromAst(AST *ast)
{
	Camera *camera = new Camera();

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

Math::Ray Camera::createRay(double x, double y, double hFOV, double vFOV)
{
	double rayX, rayY;

	rayX = tan(rad(hFOV/2)) * (2*x - 1);
	rayY = -tan(rad(vFOV/2)) * (2*y - 1);

	return Math::Ray(transformation().origin(), transformation() * Math::Vector(rayX, rayY, 1).normalize());
}

}
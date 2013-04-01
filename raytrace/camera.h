#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"
#include "Math/Ray.hpp"

class Camera : public Object
{
public:
	Camera();

	static Camera *fromAst(AST *ast);

	Math::Ray createRay(double x, double y, double hFOV, double vFOV);
};

#endif

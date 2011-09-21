#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"
#include "ray.h"

class Camera : public Object
{
public:
	Camera();

	static Camera *fromAst(AST *ast);

	Ray createRay(double x, double y, double hFOV, double vFOV);
};

#endif

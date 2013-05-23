#ifndef CAMERA_H
#define CAMERA_H

#include "Object/Base.hpp"
#include "Trace/Ray.hpp"

namespace Object {

class Camera : public Base
{
public:
	Camera();

	static Camera *fromAst(AST *ast);

	Trace::Ray createRay(double x, double y, double hFOV, double vFOV);
};

}
#endif

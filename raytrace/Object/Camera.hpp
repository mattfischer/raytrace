#ifndef CAMERA_H
#define CAMERA_H

#include "Object/Object.hpp"
#include "Math/Ray.hpp"

namespace Object {

class Camera : public Object
{
public:
	Camera();

	static Camera *fromAst(AST *ast);

	Math::Ray createRay(double x, double y, double hFOV, double vFOV);
};

}
#endif

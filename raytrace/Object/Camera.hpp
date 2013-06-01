#ifndef OBJECT_CAMERA_HPP
#define OBJECT_CAMERA_HPP

#include "Object/Base.hpp"
#include "Trace/Ray.hpp"

namespace Object {

class Camera : public Base
{
public:
	Camera(int hFOV);

	static Camera *fromAst(AST *ast);

	Trace::Ray createRay(float x, float y, float aspectRatio, int generation);

private:
	float mWidth;
};

}
#endif

#ifndef OBJECT_CAMERA_HPP
#define OBJECT_CAMERA_HPP

#include "Object/Base.hpp"
#include "Trace/Ray.hpp"

#include <memory>

namespace Object {

class Camera : public Base
{
public:
	Camera(int fov);

	static std::unique_ptr<Camera> fromAst(AST *ast);

	Trace::Ray createRay(float x, float y) const;

private:
	float mSize;
};

}
#endif

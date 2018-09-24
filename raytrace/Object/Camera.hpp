#ifndef OBJECT_CAMERA_HPP
#define OBJECT_CAMERA_HPP

#include "Object/Base.hpp"
#include "Math/Ray.hpp"

#include <memory>

namespace Object {

class Camera : public Base
{
public:
	Camera(int fov);

	static std::unique_ptr<Camera> fromAst(AST *ast);

	Math::Ray createRay(float x, float y, int generation) const;
	float projectSize(float size, float distance) const;

private:
	float mSize;
};

}
#endif

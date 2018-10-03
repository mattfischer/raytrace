#ifndef OBJECT_SCENE_HPP
#define OBJECT_SCENE_HPP

#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"

#include <vector>
#include <memory>

namespace Object {

class Scene
{
public:
	Scene(std::unique_ptr<Camera> &&camera, std::vector<std::unique_ptr<Primitive::Base>> &&primitives);

	static std::unique_ptr<Scene> fromAST(AST *ast);

	const Camera &camera() const;
	const std::vector<std::unique_ptr<Primitive::Base>> &primitives() const;
	const std::vector<std::reference_wrapper<Primitive::Base>> &lights() const;

protected:
	std::unique_ptr<Camera> mCamera;
	std::vector<std::unique_ptr<Primitive::Base>> mPrimitives;
	std::vector<std::reference_wrapper<Primitive::Base>> mLights;
};

}
#endif

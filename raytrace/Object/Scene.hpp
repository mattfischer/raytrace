#ifndef OBJECT_SCENE_HPP
#define OBJECT_SCENE_HPP

#include "Object/Camera.hpp"
#include "Object/Primitive.hpp"
#include "Object/Light.hpp"

#include <vector>
#include <memory>

namespace Object {

class Scene
{
public:
	Scene(std::unique_ptr<Camera> &&camera, std::vector<std::unique_ptr<Primitive>> &&primitives, std::vector<std::unique_ptr<Light>> &&lights);

	static std::unique_ptr<Scene> fromAST(AST *ast);

	const Camera &camera() const;
	const std::vector<std::unique_ptr<Primitive>> &primitives() const;
	const std::vector<std::reference_wrapper<Primitive>> &areaLights() const;
	const std::vector<std::unique_ptr<Light>> &lights() const;

protected:
	std::unique_ptr<Camera> mCamera;
	std::vector<std::unique_ptr<Primitive>> mPrimitives;
	std::vector<std::reference_wrapper<Primitive>> mAreaLights;
	std::vector<std::unique_ptr<Light>> mLights;
};

}
#endif

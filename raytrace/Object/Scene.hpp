#ifndef OBJECT_SCENE_HPP
#define OBJECT_SCENE_HPP

#include "Object/Forwards.hpp"
#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"
#include "Object/Light.hpp"

#include "Trace/Intersection.hpp"

#include <vector>
#include <memory>

namespace Object {

class Scene
{
public:
	static std::unique_ptr<Scene> fromAST(AST *ast);

	const Camera &camera() const;
	void setCamera(std::unique_ptr<Camera> &&camera);

	const std::vector<std::unique_ptr<Light>> &lights() const;
	void addLight(std::unique_ptr<Light> &&light);

	const std::vector<std::unique_ptr<Primitive::Base>> &primitives() const;
	void addPrimitive(std::unique_ptr<Primitive::Base> &&primitive);

	void intersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;

protected:
	std::unique_ptr<Camera> mCamera;
	std::vector<std::unique_ptr<Light>> mLights;
	std::vector<std::unique_ptr<Primitive::Base>> mPrimitives;
};

}
#endif

#ifndef OBJECT_SCENE_HPP
#define OBJECT_SCENE_HPP

#include "Object/Forwards.hpp"
#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"

#include "Trace/Intersection.hpp"

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

	void intersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;

protected:
	std::unique_ptr<Camera> mCamera;
	std::vector<std::unique_ptr<Primitive::Base>> mPrimitives;
};

}
#endif

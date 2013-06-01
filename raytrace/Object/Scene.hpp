#ifndef OBJECT_SCENE_HPP
#define OBJECT_SCENE_HPP

#include "Object/Camera.hpp"
#include "Object/Light.hpp"
#include "Object/Primitive/Base.hpp"

#include "Trace/Intersection.hpp"

#include <vector>

namespace Object {

class Scene
{
public:
	Scene();
	~Scene();

	static Scene *fromAST(AST *ast);

	Camera *camera() const;
	void setCamera(Camera *camera);

	const std::vector<Light*> &lights() const;
	void addLight(Light *light);

	const std::vector<Primitive::Base*> &primitives() const;
	void addPrimitive(Primitive::Base *primitive);

	void findIntersections(const Trace::Ray &ray, Trace::IntersectionVector &intersections);

protected:
	Camera *mCamera;
	std::vector<Light*> mLights;
	std::vector<Primitive::Base*> mPrimitives;
};

}
#endif

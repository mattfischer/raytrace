#ifndef SCENE_H
#define SCENE_H

#include "Object/Camera.hpp"
#include "Object/Light.hpp"
#include "Object/Primitive.hpp"

#include "intersection.h"

#include <vector>

class Scene
{
public:
	Scene();
	~Scene();

	Object::Camera *camera() const;
	void setCamera(Object::Camera *camera);

	const std::vector<Object::Light*> &lights() const;
	void addLight(Object::Light *light);

	const std::vector<Object::Primitive*> &primitives() const;
	void addPrimitive(Object::Primitive *primitive);

	void findIntersections(const Math::Ray &ray, std::vector<Intersection> &intersections);

protected:
	Object::Camera *mCamera;
	std::vector<Object::Light*> mLights;
	std::vector<Object::Primitive*> mPrimitives;
};

#endif

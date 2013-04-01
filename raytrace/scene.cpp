#include "scene.h"

#include <algorithm>

Scene::Scene()
{
	mCamera = 0;
}

Scene::~Scene()
{
	int i;

	if(mCamera)
		delete mCamera;

	for(i=0; i<mLights.size(); i++)
		delete mLights[i];

	for(i=0; i<mPrimitives.size(); i++)
		delete mPrimitives[i];
}

Object::Camera *Scene::camera() const
{
	return mCamera;
}

void Scene::setCamera(Object::Camera *camera)
{
	if(mCamera)
		delete mCamera;

	mCamera = camera;
}

const std::vector<Object::Light*> &Scene::lights() const
{
	return mLights;
}

void Scene::addLight(Object::Light *light)
{
	mLights.push_back(light);
}

const std::vector<Object::Primitive*> &Scene::primitives() const
{
	return mPrimitives;
}

void Scene::addPrimitive(Object::Primitive *primitive)
{
	mPrimitives.push_back(primitive);
}

void Scene::findIntersections(const Math::Ray &ray, std::vector<Intersection> &intersections)
{
	int i;
	
	for(i=0; i<mPrimitives.size(); i++)
	{
		mPrimitives[i]->intersect(ray, intersections);
	}

	std::sort(intersections.begin(), intersections.end());
}

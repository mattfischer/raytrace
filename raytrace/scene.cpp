#include "scene.h"

#include <algorithm>

Scene::Scene(double aspectRatio)
{
	mCamera = 0;
	mAspectRatio = aspectRatio;
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

Camera *Scene::camera() const
{
	return mCamera;
}

void Scene::setCamera(Camera *camera)
{
	if(mCamera)
		delete mCamera;

	mCamera = camera;
	mCamera->setAspectRatio(mAspectRatio);
}

const std::vector<Light*> &Scene::lights() const
{
	return mLights;
}

void Scene::addLight(Light *light)
{
	mLights.push_back(light);
}

const std::vector<Primitive*> &Scene::primitives() const
{
	return mPrimitives;
}

void Scene::addPrimitive(Primitive *primitive)
{
	mPrimitives.push_back(primitive);
}

void Scene::findIntersections(const Ray &ray, std::vector<Intersection> &intersections)
{
	int i;
	
	for(i=0; i<mPrimitives.size(); i++)
	{
		mPrimitives[i]->intersect(ray, intersections);
	}

	std::sort(intersections.begin(), intersections.end());
}

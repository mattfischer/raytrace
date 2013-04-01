#include "Object/Scene.hpp"

#include <algorithm>

namespace Object {

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

Scene *Scene::fromAST(AST *ast)
{
	Scene *scene = new Scene;

	for(int i=0; i<ast->numChildren; i++)
	{
		AST *child = ast->children[i];

		switch(child->type)
		{
		case AstPrimitive:
			scene->addPrimitive(Primitive::Base::fromAst(child));
			break;
		case AstLight:
			scene->addLight(Light::fromAst(child));
			break;
		case AstCamera:
			scene->setCamera(Camera::fromAst(child));
			break;
		}
	}

	return scene;
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
}

const std::vector<Light*> &Scene::lights() const
{
	return mLights;
}

void Scene::addLight(Light *light)
{
	mLights.push_back(light);
}

const std::vector<Primitive::Base*> &Scene::primitives() const
{
	return mPrimitives;
}

void Scene::addPrimitive(Primitive::Base *primitive)
{
	mPrimitives.push_back(primitive);
}

void Scene::findIntersections(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections)
{
	int i;
	
	for(i=0; i<mPrimitives.size(); i++)
	{
		mPrimitives[i]->intersect(ray, intersections);
	}

	std::sort(intersections.begin(), intersections.end());
}

}
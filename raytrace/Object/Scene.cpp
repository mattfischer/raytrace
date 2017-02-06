#include "Object/Scene.hpp"

#include "Object/Light.hpp"
#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"

namespace Object {

Scene::Scene()
{
}

Scene::~Scene()
{
}

std::unique_ptr<Scene> Scene::fromAST(AST *ast)
{
	std::unique_ptr<Scene> scene = std::make_unique<Scene>();

	for(int i=0; i<ast->numChildren; i++)
	{
		AST *child = ast->children[i];

		switch(child->type)
		{
		case AstPrimitive:
			scene->addPrimitive(std::unique_ptr<Primitive::Base>(Primitive::Base::fromAst(child)));
			break;
		case AstLight:
			scene->addLight(std::unique_ptr<Light>(Light::fromAst(child)));
			break;
		case AstCamera:
			scene->setCamera(std::unique_ptr<Camera>(Camera::fromAst(child)));
			break;
		}
	}

	return scene;
}

const Camera &Scene::camera() const
{
	return *mCamera;
}

void Scene::setCamera(std::unique_ptr<Camera> &&camera)
{
	mCamera = std::move(camera);
}

const std::vector<std::unique_ptr<Light>> &Scene::lights() const
{
	return mLights;
}

void Scene::addLight(std::unique_ptr<Light> &&light)
{
	mLights.push_back(std::move(light));
}

const std::vector<std::unique_ptr<Primitive::Base>> &Scene::primitives() const
{
	return mPrimitives;
}

void Scene::addPrimitive(std::unique_ptr<Primitive::Base> &&primitive)
{
	mPrimitives.push_back(std::move(primitive));
}

void Scene::intersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
{
	for(int i=0; i<mPrimitives.size(); i++)
	{
		mPrimitives[i]->intersect(ray, intersections);
	}
}

}
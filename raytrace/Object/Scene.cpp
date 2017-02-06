#include "Object/Scene.hpp"

#include "Object/Light.hpp"
#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"

namespace Object {

Scene::Scene(std::unique_ptr<Camera> &&camera, std::vector<std::unique_ptr<Light>> &&lights, std::vector<std::unique_ptr<Primitive::Base>> &&primitives)
	: mCamera(std::move(camera))
	, mLights(std::move(lights))
	, mPrimitives(std::move(primitives))
{
}

std::unique_ptr<Scene> Scene::fromAST(AST *ast)
{
	std::unique_ptr<Camera> camera;
	std::vector<std::unique_ptr<Light>> lights;
	std::vector<std::unique_ptr<Primitive::Base>> primitives;

	for(int i=0; i<ast->numChildren; i++)
	{
		AST *child = ast->children[i];

		switch(child->type)
		{
		case AstPrimitive:
			primitives.push_back(Primitive::Base::fromAst(child));
			break;
		case AstLight:
			lights.push_back(Light::fromAst(child));
			break;
		case AstCamera:
			camera = Camera::fromAst(child);
			break;
		}
	}

	std::unique_ptr<Scene> scene = std::make_unique<Scene>(std::move(camera), std::move(lights), std::move(primitives));

	return scene;
}

const Camera &Scene::camera() const
{
	return *mCamera;
}

const std::vector<std::unique_ptr<Light>> &Scene::lights() const
{
	return mLights;
}

const std::vector<std::unique_ptr<Primitive::Base>> &Scene::primitives() const
{
	return mPrimitives;
}

void Scene::intersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
{
	for(int i=0; i<mPrimitives.size(); i++)
	{
		mPrimitives[i]->intersect(ray, intersections);
	}
}

}
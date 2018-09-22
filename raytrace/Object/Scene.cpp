#include "Object/Scene.hpp"

#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"

namespace Object {

Scene::Scene(std::unique_ptr<Camera> &&camera, std::vector<std::unique_ptr<Primitive::Base>> &&primitives)
	: mCamera(std::move(camera))
	, mPrimitives(std::move(primitives))
{
}

std::unique_ptr<Scene> Scene::fromAST(AST *ast)
{
	std::unique_ptr<Camera> camera;
	std::vector<std::unique_ptr<Primitive::Base>> primitives;

	for(int i=0; i<ast->numChildren; i++)
	{
		AST *child = ast->children[i];

		switch(child->type)
		{
		case AstPrimitive:
			primitives.push_back(Primitive::Base::fromAst(child));
			break;
		case AstCamera:
			camera = Camera::fromAst(child);
			break;
		}
	}

	std::unique_ptr<Scene> scene = std::make_unique<Scene>(std::move(camera), std::move(primitives));

	return scene;
}

const Camera &Scene::camera() const
{
	return *mCamera;
}

const std::vector<std::unique_ptr<Primitive::Base>> &Scene::primitives() const
{
	return mPrimitives;
}

void Scene::intersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
{
	Object::Primitive::BoundingVolume::RayData rayData = Object::Primitive::BoundingVolume::getRayData(ray);

	for(int i=0; i<mPrimitives.size(); i++)
	{
		if (mPrimitives[i]->boundingVolume().intersectRay(rayData)) {
			mPrimitives[i]->intersect(ray, intersections);
		}
	}
}

}
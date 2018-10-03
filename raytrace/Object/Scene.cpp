#include "Object/Scene.hpp"

#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"
#include "Object/Light.hpp"

namespace Object {

Scene::Scene(std::unique_ptr<Camera> &&camera, std::vector<std::unique_ptr<Primitive::Base>> &&primitives, std::vector<std::unique_ptr<Light>> &&lights)
	: mCamera(std::move(camera))
	, mPrimitives(std::move(primitives))
	, mLights(std::move(lights))
{
	for (std::unique_ptr<Primitive::Base> &primitive : mPrimitives) {
		if (primitive->surface().radiance().magnitude() > 0) {
			mAreaLights.push_back(static_cast<Object::Primitive::Sampleable&>(*primitive));
		}
	}
}

std::unique_ptr<Scene> Scene::fromAST(AST *ast)
{
	std::unique_ptr<Camera> camera;
	std::vector<std::unique_ptr<Primitive::Base>> primitives;
	std::vector<std::unique_ptr<Light>> lights;

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
		case AstLight:
			lights.push_back(Light::fromAst(child));
			break;
		}
	}

	std::unique_ptr<Scene> scene = std::make_unique<Scene>(std::move(camera), std::move(primitives), std::move(lights));

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

const std::vector<std::reference_wrapper<Primitive::Sampleable>> &Scene::areaLights() const
{
	return mAreaLights;
}

const std::vector<std::unique_ptr<Light>> &Scene::lights() const
{
	return mLights;
}

}
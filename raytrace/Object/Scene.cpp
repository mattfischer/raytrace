#include "Object/Scene.hpp"

#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"
#include "Object/Light.hpp"

#include "Object/Primitive/Grid.hpp"
#include "Object/Albedo/Solid.hpp"
#include "Object/Brdf/Lambert.hpp"

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

	std::vector<Math::Point> points = { { -6, 1, 6 },   { -3, 2, 6 },   { 0, 4, 6 },   { 3, 7, 6 },   { 6, 11, 6 },
										{ -6, -3, 3 },  { -3, -2, 3 },  { 0, 0, 3 },   { 3, 3, 3 },   { 6, 7, 3 },
										{ -6, -6, 0 },  { -3, -5, 0 },  { 0, -3, 0 },  { 3, 0, 0 },   { 6, 4, 0 },
										{ -6, -8, -3 }, { -3, -7, -3 }, { 0, -5, -3 }, { 3, -2, -3 }, { 6, 2, -3 },
										{ -6, -9, -6 }, { -3, -8, -6 }, { 0, -6, -6 }, { 3, -3, -6 }, { 6, 1, -6 },
									  };
	std::unique_ptr<Object::Primitive::Base> grid = std::make_unique<Object::Primitive::Grid>(5, 5, std::move(points));
	std::unique_ptr<Object::Albedo::Base> albedo = std::make_unique<Object::Albedo::Solid>(Object::Color(0, 0.5, 1.0));
	std::unique_ptr<Object::Brdf::Composite> brdf = std::make_unique<Object::Brdf::Composite>(std::make_unique<Object::Brdf::Lambert>(1.0f), std::unique_ptr<Object::Brdf::Base>());
	std::unique_ptr<Object::Surface> surface = std::make_unique<Object::Surface>(std::move(albedo), std::move(brdf), Object::Radiance(0, 0, 0));
	grid->setSurface(std::move(surface));
	primitives.push_back(std::move(grid));

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
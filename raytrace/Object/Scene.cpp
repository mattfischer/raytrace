#include "Object/Scene.hpp"

#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"
#include "Object/Light.hpp"

#include "Object/Primitive/Model.hpp"
#include "Object/Primitive/BezierPatch.hpp"
#include "Object/Primitive/Grid.hpp"
#include "Object/Albedo/Solid.hpp"
#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"

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

	Object::Primitive::Model model("teapot.bpt");
	for (const std::unique_ptr<Object::Primitive::BezierPatch> &patch : model.patches()) {
		std::unique_ptr<Object::Primitive::Base> grid = patch->tesselate(16, 16);
		std::unique_ptr<Object::Albedo::Base> albedo = std::make_unique<Object::Albedo::Solid>(Object::Color(0, 0.5, 1.0));
		std::unique_ptr<Object::Brdf::Base> diffuse = std::make_unique<Object::Brdf::Lambert>(0.9f);
		std::unique_ptr<Object::Brdf::Base> specular; //= std::make_unique<Object::Brdf::Phong>(0.1f, 1000.0f);
		std::unique_ptr<Object::Brdf::Composite> brdf = std::make_unique<Object::Brdf::Composite>(std::move(diffuse), std::move(specular));
		std::unique_ptr<Object::Surface> surface = std::make_unique<Object::Surface>(std::move(albedo), std::move(brdf), Object::Radiance(0, 0, 0));
		grid->setSurface(std::move(surface));
		grid->transform(Math::Transformation::translate(Math::Vector(4, -9, -5)));
		grid->transform(Math::Transformation::rotate(Math::Vector(-90, 0, 0)));
		grid->transform(Math::Transformation::rotate(Math::Vector(0, 0, -45)));
		grid->transform(Math::Transformation::uniformScale(1.5));
		//grid->transform(Math::Transformation::rotate(Math::Vector(0, 90, 0)));
		grid->computeBoundingVolume();
		primitives.push_back(std::move(grid));
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
#include "Object/Scene.hpp"

#include "Object/Camera.hpp"
#include "Object/Primitive.hpp"
#include "Object/Light.hpp"

#include "Object/Shape/Transformed.hpp"
#include "Object/Albedo/Solid.hpp"
#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"

namespace Object {
	Scene::Scene(std::unique_ptr<Camera> &&camera, std::vector<std::unique_ptr<Primitive>> &&primitives, std::vector<std::unique_ptr<Light>> &&lights)
		: mCamera(std::move(camera))
		, mPrimitives(std::move(primitives))
		, mLights(std::move(lights))
	{
		std::vector<Math::Point> centroids;
		centroids.reserve(mPrimitives.size());

		for (std::unique_ptr<Primitive> &primitive : mPrimitives) {
			centroids.push_back(primitive->boundingVolume().centroid());

			if (primitive->surface().radiance().magnitude() > 0) {
				mAreaLights.push_back(static_cast<Object::Primitive&>(*primitive));
			}
		}

		auto func = [&](int index) {
			return mPrimitives[index]->boundingVolume();
		};

		mBoundingVolumeHierarchy = Object::BoundingVolumeHierarchy(std::move(centroids), func);
	}

	const Camera &Scene::camera() const
	{
		return *mCamera;
	}

	const std::vector<std::unique_ptr<Primitive>> &Scene::primitives() const
	{
		return mPrimitives;
	}

	const std::vector<std::reference_wrapper<Primitive>> &Scene::areaLights() const
	{
		return mAreaLights;
	}

	const std::vector<std::unique_ptr<Light>> &Scene::lights() const
	{
		return mLights;
	}

	const Object::BoundingVolumeHierarchy &Scene::boundingVolumeHierarchy() const
	{
		return mBoundingVolumeHierarchy;
	}
}
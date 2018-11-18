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

	Object::Intersection Scene::intersect(const Math::Beam &beam) const
	{
		Object::BoundingVolume::RayData rayData = Object::BoundingVolume::getRayData(beam.ray());

		/*auto func = [&](int index, float &maxDistance) {
			if (mPrimitives[index]->shape().intersect(ray, shapeIntersection)) {
				primitive = mPrimitives[index].get();
				return true;
			}

			return false;
		};

		mBoundingVolumeHierarchy.intersect(rayData, shapeIntersection.distance, std::ref(func));*/

		Object::Shape::Base::Intersection shapeIntersection;
		shapeIntersection.distance = FLT_MAX;
		Object::Primitive *primitive = 0;

		for (const std::unique_ptr<Object::Primitive> &testPrimitive : mPrimitives)
		{
			float volumeDistance;
			if (testPrimitive->boundingVolume().intersectRay(rayData, volumeDistance) && volumeDistance < shapeIntersection.distance) {
				if (testPrimitive->shape().intersect(beam.ray(), shapeIntersection)) {
					primitive = testPrimitive.get();
				}
			}
		}

		if (primitive) {
			return Object::Intersection(*primitive, beam, shapeIntersection);
		}
		else {
			return Object::Intersection();
		}
	}
}
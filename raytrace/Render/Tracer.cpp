#include "Render/Tracer.hpp"
#include "Object/Scene.hpp"
#include "Object/Camera.hpp"
#include "Object/Surface.hpp"

#include "Object/Brdf/Base.hpp"
#include "Object/Albedo/Base.hpp"
#include "Object/Primitive.hpp"

#include <algorithm>

namespace Render {
	Tracer::Tracer(const Object::Scene &scene, int width, int height)
		: mScene(scene)
	{
		mWidth = width;
		mHeight = height;
	}

	const Object::Scene &Tracer::scene() const
	{
		return mScene;
	}

	Intersection Tracer::intersect(const Math::Ray &ray)
	{
		Intersection intersection;
		Object::Primitive *primitive = 0;

		Object::Shape::Base::Intersection shapeIntersection;
		shapeIntersection.distance = FLT_MAX;

		Object::BoundingVolume::RayData rayData = Object::BoundingVolume::getRayData(ray);

		for (const std::unique_ptr<Object::Primitive> &testPrimitive : mScene.primitives())
		{
			float volumeDistance;
			if (testPrimitive->boundingVolume().intersectRay(rayData, volumeDistance) && volumeDistance < shapeIntersection.distance) {
				if(testPrimitive->shape().intersect(ray, shapeIntersection)) {
					primitive = testPrimitive.get();
				}
			}
		}

		if (shapeIntersection.distance < FLT_MAX) {
			Math::Point point = ray.origin() + ray.direction() * shapeIntersection.distance;
			Object::Color albedo = primitive->surface().albedo().color(shapeIntersection.surfacePoint);
			Math::Normal normal = shapeIntersection.normal;
			if (primitive->surface().hasNormalMap()) {
				normal = primitive->surface().normalMap().perturbNormal(shapeIntersection.surfacePoint, normal, shapeIntersection.tangent);
			}
			return Intersection(*primitive, ray, point, shapeIntersection.distance, normal, albedo);
		}
		else {
			return Intersection();
		}
	}

	Math::Ray Tracer::createCameraRay(float x, float y)
	{
		float cx = (2 * x - mWidth) / mWidth;
		float cy = (2 * y - mHeight) / mWidth;
		Math::Ray ray = mScene.camera().createRay(cx, -cy);

		return ray;
	}

	float Tracer::projectedPixelSize(float distance)
	{
		return mScene.camera().projectSize(2.0f / mWidth, distance);
	}
}
#include "Render/Tracer.hpp"
#include "Object/Base.hpp"
#include "Object/Scene.hpp"
#include "Object/Camera.hpp"
#include "Object/Surface.hpp"

#include "Object/Brdf/Base.hpp"
#include "Object/Albedo/Base.hpp"
#include "Object/Primitive/Base.hpp"

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

Object::Intersection Tracer::intersect(const Math::Ray &ray)
{
	Object::Intersection intersection;
	Object::Primitive::Base *primitive = 0;
	float distance = FLT_MAX;
	Math::Normal normal;

	float newDistance;
	Math::Normal newNormal;

	Object::Primitive::BoundingVolume::RayData rayData = Object::Primitive::BoundingVolume::getRayData(ray);

	for (const std::unique_ptr<Object::Primitive::Base> &testPrimitive : mScene.primitives())
	{
		float volumeDistance;
		if (testPrimitive->boundingVolume().intersectRay(rayData, volumeDistance) && volumeDistance < distance) {
			newDistance = testPrimitive->intersect(ray, newNormal);
			if (newDistance < distance) {
				primitive = testPrimitive.get();
				distance = newDistance;
				normal = newNormal;
			}
		}
	}

	if (distance < FLT_MAX) {
		return Object::Intersection(*primitive, ray, distance, normal);
	}
	else {
		return Object::Intersection();
	}
}

Math::Ray Tracer::createCameraRay(float x, float y)
{
	float cx = (2 * x - mWidth) / mWidth;
	float cy = (2 * y - mHeight) / mWidth;
	Math::Ray ray = mScene.camera().createRay(cx, cy);

	return ray;
}

float Tracer::projectedPixelSize(float distance)
{
	return mScene.camera().projectSize(2.0f / mWidth, distance);
}

}
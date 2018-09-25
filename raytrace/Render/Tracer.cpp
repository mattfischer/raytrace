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

	Object::Primitive::BoundingVolume::RayData rayData = Object::Primitive::BoundingVolume::getRayData(ray);

	for (const std::unique_ptr<Object::Primitive::Base> &primitive : mScene.primitives())
	{
		if (primitive->boundingVolume().intersectRay(rayData)) {
			Object::Intersection newIntersection = primitive->intersect(ray);
			if (!intersection.valid() || newIntersection.distance() < intersection.distance()) {
				intersection = newIntersection;
			}
		}
	}

	return intersection;
}

Math::Ray Tracer::createCameraRay(float x, float y)
{
	float cx = (2 * x - mWidth) / mWidth;
	float cy = (2 * y - mHeight) / mWidth;
	Math::Ray ray = mScene.camera().createRay(cx, cy, 1);

	return ray;
}

float Tracer::projectedPixelSize(float distance)
{
	return mScene.camera().projectSize(2.0f / mWidth, distance);
}

}
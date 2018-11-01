#include "Render/Tracer.hpp"
#include "Object/Scene.hpp"
#include "Object/Camera.hpp"
#include "Object/Surface.hpp"

#include "Object/Brdf/Base.hpp"
#include "Object/Albedo/Base.hpp"
#include "Object/Primitive.hpp"

#include "Math/Bivector2D.hpp"

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

	Intersection Tracer::intersect(const Render::Beam &beam)
	{
		const Math::Ray &ray = beam.ray();

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
			Math::Bivector projection = beam.project(shapeIntersection.distance, shapeIntersection.normal);
			Math::Vector2D du(projection.u() * shapeIntersection.tangent.u(), projection.u() * shapeIntersection.tangent.v());
			Math::Vector2D dv(projection.v() * shapeIntersection.tangent.u(), projection.v() * shapeIntersection.tangent.v());
			Math::Bivector2D surfaceProjection(du, dv);
			Object::Color albedo = primitive->surface().albedo().color(shapeIntersection.surfacePoint, surfaceProjection);
			Math::Normal normal = shapeIntersection.normal;
			if (primitive->surface().hasNormalMap()) {
				normal = primitive->surface().normalMap().perturbNormal(shapeIntersection.surfacePoint, normal, shapeIntersection.tangent);
			}
			return Intersection(*primitive, beam, point, shapeIntersection.distance, normal, albedo);
		}
		else {
			return Intersection();
		}
	}

	Beam Tracer::createCameraPixelBeam(float x, float y)
	{
		float cx = (2 * x - mWidth) / mWidth;
		float cy = (2 * y - mHeight) / mWidth;
		Math::Bivector dv;
		Math::Ray ray = mScene.camera().createRay(cx, -cy, dv);

		float pixelSize = 2.0f / mWidth;

		return Beam(ray, Math::Bivector(), dv * pixelSize);
	}

	float Tracer::projectedPixelSize(float distance)
	{
		return mScene.camera().projectSize(2.0f / mWidth, distance);
	}
}
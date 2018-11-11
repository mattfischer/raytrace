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

		auto func = [&](int index, float &maxDistance) {
			if (mScene.primitives()[index]->shape().intersect(ray, shapeIntersection)) {
				primitive = mScene.primitives()[index].get();
				return true;
			}

			return false;
		};

		bool valid = false;

		//valid = mScene.boundingVolumeHierarchy().intersect(rayData, shapeIntersection.distance, std::ref(func));

		for (const std::unique_ptr<Object::Primitive> &testPrimitive : mScene.primitives())
		{
			float volumeDistance;
			if (testPrimitive->boundingVolume().intersectRay(rayData, volumeDistance) && volumeDistance < shapeIntersection.distance) {
				if (testPrimitive->shape().intersect(ray, shapeIntersection)) {
					primitive = testPrimitive.get();
					valid = true;
				}
			}
		}

		if (valid) {
			Math::Point point = ray.origin() + ray.direction() * shapeIntersection.distance;
			Math::Bivector projection = beam.project(shapeIntersection.distance, shapeIntersection.normal);
			Math::Vector v = shapeIntersection.tangent.u() % shapeIntersection.tangent.v();
			v = v / v.magnitude2();
			Math::Vector2D du((projection.u() % shapeIntersection.tangent.v()) * v, (shapeIntersection.tangent.u() % projection.u()) * v);
			Math::Vector2D dv((projection.v() % shapeIntersection.tangent.v()) * v, (shapeIntersection.tangent.u() % projection.v()) * v);
			Math::Bivector2D surfaceProjection(du, dv);
			Object::Color albedo = primitive->surface().albedo().color(shapeIntersection.surfacePoint, surfaceProjection);
			Math::Normal normal = shapeIntersection.normal;
			if (primitive->surface().hasNormalMap()) {
				normal = primitive->surface().normalMap().perturbNormal(shapeIntersection.surfacePoint, surfaceProjection, normal, shapeIntersection.tangent);
			}
			return Intersection(*primitive, beam, point, shapeIntersection.distance, normal, albedo);
		}
		else {
			return Intersection();
		}
	}

	Beam Tracer::createCameraPixelBeam(const Math::Point2D &imagePoint, const Math::Point2D &aperturePoint)
	{
		float cx = (2 * imagePoint.u() - mWidth) / mWidth;
		float cy = (2 * imagePoint.v() - mHeight) / mWidth;
		Math::Bivector dv;
		Math::Point2D imagePointTransformed(cx, -cy);
		Math::Ray ray = mScene.camera().createRay(imagePointTransformed, aperturePoint, dv);

		float pixelSize = 2.0f / mWidth;

		return Beam(ray, Math::Bivector(), dv * pixelSize);
	}

	float Tracer::projectedPixelSize(float distance)
	{
		return mScene.camera().projectSize(2.0f / mWidth, distance);
	}
}
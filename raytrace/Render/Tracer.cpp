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
	Tracer::Tracer(const Object::Scene &scene, int width, int height, Sampler &sampler)
		: mScene(scene), mSampler(sampler)
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

		Object::Primitive *primitive = 0;
		Object::Shape::Base::Intersection shapeIntersection;
		shapeIntersection.distance = FLT_MAX;

		bool valid = mScene.intersect(ray, shapeIntersection, primitive);

		if (valid) {
			return Intersection(*primitive, beam, shapeIntersection);
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

	Sampler &Tracer::sampler()
	{
		return mSampler;
	}
}
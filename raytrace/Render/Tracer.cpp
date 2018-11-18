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

	Intersection Tracer::intersect(const Math::Beam &beam)
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

	Math::Beam Tracer::createCameraPixelBeam(const Math::Point2D &imagePoint, const Math::Point2D &aperturePoint)
	{
		return mScene.camera().createPixelBeam(imagePoint, mWidth, mHeight, aperturePoint);
	}

	Sampler &Tracer::sampler()
	{
		return mSampler;
	}
}
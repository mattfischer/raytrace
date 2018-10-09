#include "Lighter/Specular.hpp"

#include "Render/Tracer.hpp"
#include "Object/Scene.hpp"

#include "Render/Engine.hpp"

namespace Lighter {

Specular::Specular(const Lighter::Base &lighter, int maxRayGeneration)
	: mLighter(lighter)
{
	mMaxRayGeneration = maxRayGeneration;
}

Object::Radiance Specular::light(const Object::Intersection &intersection, Render::Tracer &tracer) const
{
	const Object::Surface &surface = intersection.primitive()->surface();
	const Math::Ray &ray = intersection.ray();
	Object::Radiance radiance;

	if (surface.brdf().specular() && ray.generation() < mMaxRayGeneration) {
		Object::Color albedo = surface.albedo().color(intersection.objectPoint());
		Math::Vector incident = ray.direction();
		Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

		Math::Ray reflectRay(intersection.point(), reflect, ray.generation() + 1);
		Object::Intersection intersection2 = tracer.intersect(reflectRay);

		if (intersection2.valid())
		{
			Object::Radiance reflectRadiance = mLighter.light(intersection2, tracer);
			radiance += surface.brdf().specularRadiance(reflectRadiance, albedo);
		}
	}

	return radiance;
}

}

#include "Trace/Lighter/Specular.hpp"

#include "Trace/Tracer.hpp"
#include "Object/Scene.hpp"

#include "Render/Engine.hpp"

namespace Trace {
namespace Lighter {

Specular::Specular(const Render::Engine &engine, int maxRayGeneration)
	: mEngine(engine)
{
	mMaxRayGeneration = maxRayGeneration;
}

Object::Radiance Specular::light(const Trace::Intersection &intersection, Trace::Tracer &tracer, Probe *probe) const
{
	const Object::Surface &surface = intersection.primitive()->surface();
	const Trace::Ray &ray = intersection.ray();
	Object::Radiance radiance;

	if (surface.brdf().specular() && ray.generation() < mMaxRayGeneration) {
		Object::Color albedo = surface.albedo().color(intersection.objectPoint());
		Math::Vector incident = ray.direction();
		Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

		Trace::Ray reflectRay(intersection.point(), reflect, ray.generation() + 1);
		Object::Radiance reflectRadiance = mEngine.traceRay(reflectRay, tracer);

		radiance += surface.brdf().specularRadiance(reflectRadiance, albedo);
	}

	return radiance;
}

}
}
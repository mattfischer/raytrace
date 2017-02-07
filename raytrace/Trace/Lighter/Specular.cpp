#include "Trace/Lighter/Specular.hpp"

#include "Trace/Tracer.hpp"
#include "Object/Scene.hpp"
#include "Object/Light.hpp"

namespace Trace {
namespace Lighter {

Object::Radiance Specular::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Object::Surface &surface = intersection.primitive()->surface();
	Object::Radiance radiance;

	if (surface.brdf().specular() && tracer.generation() < tracer.settings().maxRayGeneration) {
		const Trace::Ray &ray = intersection.ray();
		Object::Color albedo = surface.albedo().color(intersection.objectPoint());
		Math::Vector incident = ray.direction();
		Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

		Trace::Ray reflectRay(intersection.point(), reflect);
		Object::Radiance reflectRadiance = tracer.traceRay(reflectRay);

		radiance += surface.brdf().specularRadiance(reflectRadiance, albedo);
	}

	return radiance;
}

}
}
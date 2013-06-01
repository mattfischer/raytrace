#include "Surface/Reflection.hpp"
#include "Trace/Ray.hpp"
#include "Trace/Tracer.hpp"

namespace Surface {

Object::Color Reflection::color(const Trace::Intersection &intersection, const Trace::Tracer &tracer) const
{
	Object::Color ret;

	const Trace::Ray &ray = intersection.ray();
	if(ray.generation() < tracer.settings().maxRayGeneration) {
		Math::Vector incident = ray.direction();
		Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

		Trace::Ray reflectRay(intersection.point(), reflect);
		reflectRay.setGeneration(ray.generation() + 1);

		ret = tracer.traceRay(reflectRay);
	}

	return ret;
}

}
#include "Trace/Lighter/Direct.hpp"

#include "Trace/Tracer.hpp"
#include "Object/Scene.hpp"
#include "Object/Light.hpp"

namespace Trace {
namespace Lighter {

Object::Radiance Direct::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	Math::Point point(intersection.point());
	const Object::Surface &surface = intersection.primitive()->surface();
	Math::Vector viewDirection = -intersection.ray().direction().normalize();
	Object::Color albedo = surface.albedo().color(intersection.objectPoint());
	Object::Radiance radiance;

	for(const std::unique_ptr<Object::Light> &light : tracer.scene().lights())
	{
		Trace::Ray shadowRay = Trace::Ray::createFromPoints(point, light->transformation().origin());

		Trace::IntersectionVector::iterator begin, end;
		tracer.intersect(shadowRay, begin, end);

		Math::Vector lightVector = light->transformation().origin() - point;
		float lightMagnitude = lightVector.magnitude();
		Math::Vector lightDir = lightVector / lightMagnitude;

		if(begin == end || begin->distance() >= lightMagnitude)
		{
			radiance += surface.brdf().radiance(light->radiance(), lightDir, intersection.normal(), viewDirection, albedo);
		}

		tracer.popTrace();
	}

	return radiance;
}

}
}
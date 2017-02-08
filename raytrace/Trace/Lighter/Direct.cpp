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
	Math::Vector outgoingDirection = -intersection.ray().direction().normalize();
	Object::Color albedo = surface.albedo().color(intersection.objectPoint());
	Object::Radiance radiance;

	for(const std::unique_ptr<Object::Light> &light : tracer.scene().lights())
	{
		Trace::Ray shadowRay = Trace::Ray::createFromPoints(point, light->transformation().origin(), intersection.ray().generation() + 1);

		Trace::IntersectionVector::iterator begin, end;
		tracer.intersect(shadowRay, begin, end);

		Math::Vector lightVector = light->transformation().origin() - point;
		float lightMagnitude = lightVector.magnitude();

		if(begin == end || begin->distance() >= lightMagnitude)
		{
			Math::Vector incidentDirection = lightVector / lightMagnitude;
			const Math::Normal &normal = intersection.normal();

			float dot = normal * incidentDirection;
			if (dot > 0) {
				Object::Radiance incidentRadiance = light->radiance() * dot / (lightMagnitude * lightMagnitude);
				radiance += surface.brdf().radiance(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo);
			}
		}

		tracer.popTrace();
	}

	return radiance;
}

}
}
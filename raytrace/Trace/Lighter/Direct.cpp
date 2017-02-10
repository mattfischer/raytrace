#define _USE_MATH_DEFINES
#include "Trace/Lighter/Direct.hpp"

#include "Trace/Lighter/Utils.hpp"

#include "Trace/Tracer.hpp"
#include "Object/Scene.hpp"
#include "Object/Light.hpp"

namespace Trace {
namespace Lighter {

Direct::Direct(int numSamples)
{
		mNumSamples = numSamples;
}

Object::Radiance Direct::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	return doPointLights(intersection, tracer) + doSampledLights(intersection, tracer);
}

Object::Radiance Direct::doPointLights(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
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
				Object::Radiance incidentRadiance = light->radiance() * dot / (4 * M_PI * lightMagnitude * lightMagnitude);
				radiance += surface.brdf().radiance(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo);
			}
		}
	}

	return radiance;
}

Object::Radiance Direct::doSampledLights(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Point &point = intersection.point();
	const Math::Vector normal(intersection.normal());
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();

	Object::Radiance radiance;
	for (const std::unique_ptr<Object::Primitive::Base> &primitive : tracer.scene().primitives()) {
		const Object::Radiance &objectRadiance = primitive->surface().radiance();
		if (objectRadiance.red() == 0 && objectRadiance.green() == 0 && objectRadiance.blue() == 0) {
			continue;
		}

		Math::Vector sphereDirection = primitive->boundingSphere().origin() - point;
		float sphereDistance = sphereDirection.magnitude();
		float sphereAngle = std::atan(primitive->boundingSphere().radius() / sphereDistance);
		sphereDirection = sphereDirection / sphereDistance;

		Math::Vector x, y;
		Utils::orthonormalBasis(sphereDirection, x, y);
		float solidAngle = 2 * M_PI * (1 - std::cos(sphereAngle));

		Object::Radiance outgoingRadiance;
		for (int i = 0; i < mNumSamples; i++) {
			Math::Vector v = Utils::sampleHemisphere(i, mNumSamples, sphereAngle, mRandomEngine);
			Math::Vector incidentDirection = x * v.x() + y * v.y() + sphereDirection * v.z();
			float dot = incidentDirection * normal;

			Trace::Ray ray(intersection.point(), incidentDirection, intersection.ray().generation() + 1);
			Trace::IntersectionVector::iterator begin, end;
			tracer.intersect(ray, begin, end);

			if (begin != end && begin->primitive() == primitive.get()) {
				outgoingRadiance += brdf.radiance(objectRadiance, incidentDirection, intersection.normal(), outgoingDirection, albedo) * dot;
			}
		}

		radiance += outgoingRadiance * (solidAngle / mNumSamples);
	}

	return radiance;
}

}
}
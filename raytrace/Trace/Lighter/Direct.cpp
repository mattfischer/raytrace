#define _USE_MATH_DEFINES
#include "Trace/Lighter/Direct.hpp"

#include "Trace/Lighter/Utils.hpp"

#include "Trace/Tracer.hpp"
#include "Object/Scene.hpp"

namespace Trace {
namespace Lighter {

Direct::Direct(int numSamples)
{
	mNumSamples = numSamples;
}

Object::Radiance Direct::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Point &point = intersection.point();
	const Math::Vector normal(intersection.normal());
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();

	Object::Radiance radiance;
	if (brdf.lambert() == 0) {
		return radiance;
	}

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
				outgoingRadiance += objectRadiance * albedo * brdf.lambert() * dot / (2 * M_PI);

				addProbeEntry(v, objectRadiance);
			} else {
				addProbeEntry(v, Object::Radiance(0, 0, 0));
			}
		}

		radiance += outgoingRadiance * (solidAngle / mNumSamples);
	}

	return radiance;
}

}
}
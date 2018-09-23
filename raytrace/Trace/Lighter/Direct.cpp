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

Object::Radiance Direct::light(const Trace::Intersection &intersection, Trace::Tracer &tracer, Probe *probe) const
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

		if(primitive->canSample()) {
			Object::Radiance outgoingRadiance;
			Math::Vector x, y;
			Utils::orthonormalBasis(normal, x, y);

			for (int i = 0; i < mNumSamples; i++) {
				float u;
				float v;

				Utils::stratifiedSamples(i, mNumSamples, u, v, mRandomEngine);

				Math::Point samplePoint;
				Math::Vector du;
				Math::Vector dv;
				Math::Normal sampleNormal;

				primitive->sample(u, v, samplePoint, du, dv, sampleNormal);
				float area = (du % dv).magnitude();

				Math::Vector incidentDirection = samplePoint - point;
				float distance = incidentDirection.magnitude();
				incidentDirection = incidentDirection / distance;
				Math::Point offsetPoint = point + Math::Vector(intersection.normal()) * 0.01;
				Trace::Ray ray(offsetPoint, incidentDirection, intersection.ray().generation() + 1);
				Trace::Intersection intersection2 = tracer.intersect(ray);

				Math::Vector viewVector(incidentDirection * x, incidentDirection * y, incidentDirection * normal);
				Probe::Entry probeEntry;
				probeEntry.direction = viewVector;
				if (intersection2.valid() && intersection2.primitive() == primitive.get()) {
					float dot = incidentDirection * normal;
					float sampleDot = abs(incidentDirection * sampleNormal);

					if (dot > 0) {
						outgoingRadiance += objectRadiance * dot * sampleDot * area / (distance * distance);
						probeEntry.radiance = objectRadiance;
					}
				}

				if(probe) {
					probe->entries.push_back(probeEntry);
				}
			}

			radiance += outgoingRadiance * albedo * brdf.lambert() / (M_PI * mNumSamples);
		}
	}

	return radiance;
}

}
}
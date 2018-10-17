#define _USE_MATH_DEFINES
#include "Lighter/DiffuseDirect.hpp"

#include "Lighter/Utils.hpp"

#include "Render/Tracer.hpp"
#include "Object/Scene.hpp"

namespace Lighter {

DiffuseDirect::DiffuseDirect(int numSamples)
{
	mNumSamples = numSamples;
}

Object::Radiance DiffuseDirect::light(const Object::Intersection &intersection, Render::Tracer &tracer, int generation) const
{
	const Math::Point &point = intersection.point();
	Math::Normal normal = intersection.normal();
	const Object::Color &albedo = intersection.primitive().surface().albedo().color(intersection.primitive().transformation().inverse() * intersection.point());
	const Object::Brdf::Base &brdf = intersection.primitive().surface().brdf().diffuse();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();
	if (outgoingDirection * normal < 0) {
		normal = -normal;
	}

	clearProbe();

	Math::Vector x, y;
	Utils::orthonormalBasis(Math::Vector(normal), x, y);

	Object::Radiance radiance;
	for (const Object::Primitive::Sampleable &sampleable : tracer.scene().areaLights()) {
		const Object::Radiance &objectRadiance = sampleable.surface().radiance();

		for (int i = 0; i < mNumSamples; i++) {
			float u;
			float v;

			Utils::stratifiedSamples(i, mNumSamples, u, v, mRandomEngine);

			Math::Point samplePoint;
			Math::Vector du;
			Math::Vector dv;
			Math::Normal sampleNormal;

			sampleable.sample(u, v, samplePoint, du, dv, sampleNormal);
			float area = (du % dv).magnitude();

			Math::Vector incidentDirection = samplePoint - point;
			float distance = incidentDirection.magnitude();
			incidentDirection = incidentDirection / distance;
			Math::Point offsetPoint = point + Math::Vector(normal) * 0.0001;
			Math::Ray ray(offsetPoint, incidentDirection);
			Object::Intersection intersection2 = tracer.intersect(ray);

			Math::Vector probeDirection(incidentDirection * x, incidentDirection * y, incidentDirection * normal);
			Object::Radiance probeRadiance;
			if (intersection2.valid() && &(intersection2.primitive()) == &sampleable) {
				float dot = incidentDirection * normal;
				float sampleDot = abs(incidentDirection * sampleNormal);

				if (dot > 0) {
					Object::Radiance incidentRadiance = objectRadiance * sampleDot * dot * area / (distance * distance);
					radiance += brdf.radiance(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / mNumSamples;
					probeRadiance = objectRadiance;
				}
			}

			addProbeEntry(probeDirection, probeRadiance);
		}
	}

	for (const std::unique_ptr<Object::Light> &light : tracer.scene().lights()) {
		Math::Point offsetPoint = point + Math::Vector(normal) * 0.01;
		Math::Vector incidentDirection = light->position() - offsetPoint;
		float distance = incidentDirection.magnitude();
		incidentDirection = incidentDirection / distance;

		Math::Ray ray(offsetPoint, incidentDirection);
		Object::Intersection intersection2 = tracer.intersect(ray);

		Math::Vector probeDirection(incidentDirection * x, incidentDirection * y, incidentDirection * normal);
		Object::Radiance probeRadiance;
		if (!intersection2.valid() || intersection2.distance() >= distance) {
			float dot = incidentDirection * normal;
			if (dot > 0) {
				Object::Radiance incidentRadiance = light->radiance() * dot / (distance * distance);
				radiance += brdf.radiance(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo);

				probeRadiance = light->radiance();
			}
		}

		addProbeEntry(probeDirection, probeRadiance);
	}

	return radiance;
}

}
#define _USE_MATH_DEFINES
#include "Lighter/DiffuseDirect.hpp"

#include "Lighter/Sampler.hpp"

#include "Math/OrthonormalBasis.hpp"

namespace Lighter {
	DiffuseDirect::DiffuseDirect(int numSamples)
	{
		mNumSamples = numSamples;
	}

	Object::Radiance DiffuseDirect::light(const Object::Intersection &intersection, Render::Tracer &tracer, int generation) const
	{
		if (!intersection.primitive().surface().brdf().hasDiffuse()) {
			return Object::Radiance();
		}

		const Math::Point &point = intersection.point();
		Math::Normal normal = intersection.normal();
		const Object::Color &albedo = intersection.albedo();
		const Object::Brdf::Base &brdf = intersection.primitive().surface().brdf().diffuse();
		const Math::Vector &outgoingDirection = -intersection.ray().direction();
		if (outgoingDirection * normal < 0) {
			normal = -normal;
		}

		clearProbe();

		Math::OrthonormalBasis basis(normal);
		Lighter::Sampler sampler(mNumSamples, mRandomEngine);

		Object::Radiance radiance;
		for (const Object::Primitive &primitive : tracer.scene().areaLights()) {
			const Object::Radiance &objectRadiance = primitive.surface().radiance();

			for (int i = 0; i < mNumSamples; i++) {
				float u;
				float v;

				sampler.sample(i, u, v);

				Math::Point samplePoint;
				Math::Vector du;
				Math::Vector dv;
				Math::Normal sampleNormal;

				primitive.shape().sample(u, v, samplePoint, du, dv, sampleNormal);
				float area = (du % dv).magnitude();

				Math::Vector incidentDirection = samplePoint - point;
				float distance = incidentDirection.magnitude();
				incidentDirection = incidentDirection / distance;
				Math::Point offsetPoint = point + Math::Vector(normal) * 0.0001;
				Math::Ray ray(offsetPoint, incidentDirection);
				Object::Intersection intersection2 = tracer.intersect(ray);

				Math::Vector probeDirection = basis.worldToLocal(incidentDirection);
				Object::Radiance probeRadiance;
				if (intersection2.valid() && &(intersection2.primitive()) == &primitive) {
					float dot = incidentDirection * normal;
					float sampleDot = abs(incidentDirection * sampleNormal);

					if (dot > 0) {
						Object::Radiance incidentRadiance = objectRadiance * sampleDot * dot * area / (distance * distance);
						Object::Radiance transmittedRadiance = incidentRadiance;
						if (intersection.primitive().surface().brdf().hasSpecular()) {
							transmittedRadiance = intersection.primitive().surface().brdf().specular().transmitted(incidentRadiance, incidentDirection, normal, albedo);
						}
						radiance += brdf.reflected(transmittedRadiance, incidentDirection, normal, outgoingDirection, albedo) / mNumSamples;
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

			Math::Vector probeDirection = basis.worldToLocal(incidentDirection);
			Object::Radiance probeRadiance;
			if (!intersection2.valid() || intersection2.distance() >= distance) {
				float dot = incidentDirection * normal;
				if (dot > 0) {
					Object::Radiance incidentRadiance = light->radiance() * dot / (distance * distance);
					Object::Radiance transmittedRadiance;
					if (intersection.primitive().surface().brdf().hasSpecular()) {
						transmittedRadiance = intersection.primitive().surface().brdf().specular().transmitted(incidentRadiance, incidentDirection, normal, albedo);
					}
					else {
						transmittedRadiance = incidentRadiance;
					}

					radiance += brdf.reflected(transmittedRadiance, incidentDirection, normal, outgoingDirection, albedo);

					probeRadiance = light->radiance();
				}
			}

			addProbeEntry(probeDirection, probeRadiance);
		}

		return radiance;
	}
}
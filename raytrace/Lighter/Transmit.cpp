#define _USE_MATH_DEFINES
#include "Lighter/Transmit.hpp"

#include <cmath>

namespace Lighter {
	Transmit::Transmit(const Lighter::Base &lighter, int maxGeneration)
		: mLighter(lighter)
	{
		mMaxGeneration = maxGeneration;
	}

	Object::Radiance Transmit::light(const Object::Intersection &intersection, Render::Tracer &tracer, int generation) const
	{
		const Object::Surface &surface = intersection.primitive().surface();
		const Math::Ray &ray = intersection.ray();
		Math::Normal normal = intersection.normal();
		const Object::Color &albedo = intersection.albedo();
		Math::Vector outgoingDirection = -ray.direction();
		bool reverse = false;

		if (normal * outgoingDirection < 0) {
			normal = -normal;
			reverse = true;
		}

		Object::Radiance radiance;

		if (surface.brdf().hasTransmit() && generation < mMaxGeneration) {
			Math::Point offsetPoint = intersection.point() - Math::Vector(normal) * 0.01;

			float ratio = 1.0f / surface.brdf().transmitIor();
			if (reverse) {
				ratio = 1.0f / ratio;
			}

			float c1 = outgoingDirection * normal;
			float c2 = std::sqrt(1.0f - ratio * ratio * (1.0f - c1 * c1));

			Math::Vector incidentDirection = Math::Vector(normal) * (ratio * c1 - c2) - outgoingDirection * ratio;
			Math::Ray transmitRay(offsetPoint, incidentDirection);
			Object::Intersection intersection2 = tracer.intersect(transmitRay);

			Math::Normal incidentNormal = -normal;
			float dot = incidentDirection * incidentNormal;
			if (intersection2.valid()) {
				Object::Radiance incidentRadiance = mLighter.light(intersection2, tracer, generation + 1) * dot;
				Object::Radiance transmittedRadiance = incidentRadiance;
				if (intersection.primitive().surface().brdf().hasSpecular()) {
					transmittedRadiance = intersection.primitive().surface().brdf().specular().transmitted(incidentRadiance, incidentDirection, incidentNormal, albedo);
				}
				if (intersection.primitive().surface().brdf().hasDiffuse()) {
					transmittedRadiance = intersection.primitive().surface().brdf().diffuse().transmitted(incidentRadiance, incidentDirection, incidentNormal, albedo);
				}

				radiance += transmittedRadiance / (outgoingDirection * normal);
			}
		}

		return radiance;
	}
}
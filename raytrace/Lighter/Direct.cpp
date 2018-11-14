#define _USE_MATH_DEFINES
#include "Lighter/Direct.hpp"

#include "Render/Sampler.hpp"

#include "Math/OrthonormalBasis.hpp"

namespace Lighter {
	Direct::Direct(int numSamples, bool misSpecular, int numSpecularSamples)
	{
		mNumSamples = numSamples;
		mMisSpecular = misSpecular;
		mNumSpecularSamples = numSpecularSamples;
	}

	Object::Radiance Direct::light(const Render::Intersection &intersection, Render::Tracer &tracer, int generation) const
	{
		bool hasDiffuse = intersection.primitive().surface().brdf().hasDiffuse();
		bool hasSpecular = intersection.primitive().surface().brdf().hasSpecular();

		if (!hasDiffuse && !hasSpecular) {
			return Object::Radiance();
		}

		const Math::Point &point = intersection.point();
		Math::Normal normal = intersection.normal();
		const Object::Color &albedo = intersection.albedo();
		
		const Object::Brdf::Base &diffuseBrdf = intersection.primitive().surface().brdf().diffuse();
		const Object::Brdf::Base &specularBrdf = intersection.primitive().surface().brdf().specular();
		const Math::Vector &outgoingDirection = -intersection.ray().direction();
		if (outgoingDirection * normal < 0) {
			normal = -normal;
		}

		clearProbe();

		Math::OrthonormalBasis basis(normal);

		Object::Radiance radiance;
		for (const Object::Primitive &light : tracer.scene().areaLights()) {
			const Object::Radiance &objectRadiance = light.surface().radiance();
			const Object::Shape::Base::Sampler *shapeSampler = light.shape().sampler();
			if (!shapeSampler) {
				continue;
			}

			float surfaceArea = shapeSampler->surfaceArea();
			for (int i = 0; i < mNumSamples; i++) {
				Math::Point2D surfacePoint = tracer.sampler().getValue2D();

				Math::Point samplePoint;
				Math::Vector du;
				Math::Vector dv;
				Math::Normal sampleNormal;

				shapeSampler->sample(surfacePoint, samplePoint, sampleNormal);

				Math::Vector incidentDirection = samplePoint - point;
				float distance = incidentDirection.magnitude();
				incidentDirection = incidentDirection / distance;

				float dot = incidentDirection * normal;
				if (dot > 0) {
					Math::Point offsetPoint = point + Math::Vector(normal) * 0.0001;
					Math::Ray ray(offsetPoint, incidentDirection);
					Render::Beam beam(ray, Math::Bivector(), Math::Bivector());
					Render::Intersection intersection2 = tracer.intersect(beam);

					Math::Vector probeDirection = basis.worldToLocal(incidentDirection);
					Object::Radiance probeRadiance;
					if (intersection2.valid() && &(intersection2.primitive()) == &light) {
						float dot = incidentDirection * normal;
						float sampleDot = abs(incidentDirection * sampleNormal);
						float pdf = 1.0f / surfaceArea;

						Object::Radiance incidentRadiance = objectRadiance * sampleDot * dot / (distance * distance);
						Object::Radiance transmittedRadiance = incidentRadiance;
						if (hasSpecular && mMisSpecular) {
							Object::Radiance reflectedRadiance = specularBrdf.reflected(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / (pdf * mNumSamples);
							float specularPdf = specularBrdf.pdf(incidentDirection, normal, outgoingDirection) * sampleDot / (distance * distance);
							float weight = (mNumSamples * pdf) / (mNumSamples * pdf + mNumSpecularSamples * specularPdf);
							radiance += reflectedRadiance * weight;
							transmittedRadiance = specularBrdf.transmitted(incidentRadiance, incidentDirection, normal, albedo);
						}

						if (hasDiffuse) {
							radiance += diffuseBrdf.reflected(transmittedRadiance, incidentDirection, normal, outgoingDirection, albedo) / (pdf * mNumSamples);
						}
						probeRadiance = objectRadiance;
					}

					addProbeEntry(probeDirection, probeRadiance);
				}
			}
		}

		for (const std::unique_ptr<Object::Light> &light : tracer.scene().lights()) {
			Math::Point offsetPoint = point + Math::Vector(normal) * 0.01;
			Math::Vector incidentDirection = light->position() - offsetPoint;
			float distance = incidentDirection.magnitude();
			incidentDirection = incidentDirection / distance;

			Math::Ray ray(offsetPoint, incidentDirection);
			Render::Beam beam(ray, Math::Bivector(), Math::Bivector());
			Render::Intersection intersection2 = tracer.intersect(beam);

			Math::Vector probeDirection = basis.worldToLocal(incidentDirection);
			Object::Radiance probeRadiance;
			if (!intersection2.valid() || intersection2.distance() >= distance) {
				float dot = incidentDirection * normal;
				if (dot > 0) {
					Object::Radiance incidentRadiance = light->radiance() * dot / (distance * distance);
					Object::Radiance transmittedRadiance = incidentRadiance;
					if (hasSpecular && mIncludeSpecular) {
						transmittedRadiance = intersection.primitive().surface().brdf().specular().transmitted(incidentRadiance, incidentDirection, normal, albedo);
					}
					
					if (hasDiffuse) {
						radiance += diffuseBrdf.reflected(transmittedRadiance, incidentDirection, normal, outgoingDirection, albedo);
					}

					probeRadiance = light->radiance();
				}
			}

			addProbeEntry(probeDirection, probeRadiance);
		}

		return radiance;
	}
}
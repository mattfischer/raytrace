#define _USE_MATH_DEFINES
#include "Lighter/Direct.hpp"

#include "Object/Scene.hpp"

#include "Render/Sampler.hpp"

#include "Math/OrthonormalBasis.hpp"

namespace Lighter {
	Direct::Direct(bool misSpecular)
	{
		mMisSpecular = misSpecular;
	}

	Object::Radiance Direct::light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
	{
		bool hasDiffuse = intersection.primitive().surface().brdf().hasDiffuse();
		bool hasSpecular = intersection.primitive().surface().brdf().hasSpecular();

		if (!hasDiffuse && !hasSpecular) {
			return Object::Radiance();
		}

		const Object::Scene &scene = intersection.scene();
		const Math::Point &point = intersection.point();
		const Math::Normal &normal = intersection.facingNormal();
		const Object::Color &albedo = intersection.albedo();
		
		const Object::Brdf::Base &diffuseBrdf = intersection.primitive().surface().brdf().diffuse();
		const Object::Brdf::Base &specularBrdf = intersection.primitive().surface().brdf().specular();
		const Math::Vector &outgoingDirection = -intersection.ray().direction();

		Math::OrthonormalBasis basis(normal);

		Object::Radiance radiance;
		for (const Object::Primitive &light : scene.areaLights()) {
			const Object::Radiance &objectRadiance = light.surface().radiance();
			const Object::Shape::Base::Sampler *shapeSampler = light.shape().sampler();
			if (!shapeSampler) {
				continue;
			}

			float surfaceArea = shapeSampler->surfaceArea();
			Math::Point2D surfacePoint = sampler.getValue2D();

			Math::Point samplePoint;
			Math::Normal sampleNormal;

			shapeSampler->sample(surfacePoint, samplePoint, sampleNormal);

			Math::Vector incidentDirection = samplePoint - point;
			float distance = incidentDirection.magnitude();
			incidentDirection = incidentDirection / distance;

			float dot = incidentDirection * normal;
			if (dot > 0) {
				Math::Point offsetPoint = point + Math::Vector(normal) * 0.0001;
				Math::Ray ray(offsetPoint, incidentDirection);
				Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
				Object::Intersection intersection2 = scene.intersect(beam);

				if (intersection2.valid() && &(intersection2.primitive()) == &light) {
					float dot = incidentDirection * normal;
					float sampleDot = abs(incidentDirection * sampleNormal);
					float pdf = 1.0f / surfaceArea;

					Object::Radiance incidentRadiance = objectRadiance * sampleDot * dot / (distance * distance);
					Object::Radiance transmittedRadiance = incidentRadiance;
					if (hasSpecular && mMisSpecular) {
						Object::Radiance reflectedRadiance = specularBrdf.reflected(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / pdf;
						float specularPdf = specularBrdf.pdf(incidentDirection, normal, outgoingDirection) * sampleDot / (distance * distance);
						float weight = pdf / (pdf + specularPdf);
						radiance += reflectedRadiance * weight;
						transmittedRadiance = specularBrdf.transmitted(incidentRadiance, incidentDirection, normal, albedo);
					}

					if (hasDiffuse) {
						radiance += diffuseBrdf.reflected(transmittedRadiance, incidentDirection, normal, outgoingDirection, albedo) / pdf;
					}
				}
			}
		}

		for (const std::unique_ptr<Object::PointLight> &pointLight : scene.pointLights()) {
			Math::Point offsetPoint = point + Math::Vector(normal) * 0.01;
			Math::Vector incidentDirection = pointLight->position() - offsetPoint;
			float distance = incidentDirection.magnitude();
			incidentDirection = incidentDirection / distance;

			Math::Ray ray(offsetPoint, incidentDirection);
			Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
			Object::Intersection intersection2 = scene.intersect(beam);

			if (!intersection2.valid() || intersection2.distance() >= distance) {
				float dot = incidentDirection * normal;
				if (dot > 0) {
					Object::Radiance incidentRadiance = pointLight->radiance() * dot / (distance * distance);
					Object::Radiance transmittedRadiance = incidentRadiance;
					if (hasSpecular && mMisSpecular) {
						transmittedRadiance = intersection.primitive().surface().brdf().specular().transmitted(incidentRadiance, incidentDirection, normal, albedo);
					}
					
					if (hasDiffuse) {
						radiance += diffuseBrdf.reflected(transmittedRadiance, incidentDirection, normal, outgoingDirection, albedo);
					}
				}
			}
		}

		return radiance;
	}

	Object::Radiance Direct::sampleIrradiance(const Object::Intersection &intersection, const Math::OrthonormalBasis &basis, Render::Sampler &sampler, Math::Vector &localIncidentDirection) const
	{
		const Object::Scene &scene = intersection.scene();
		const Math::Point &point = intersection.point();
		const Math::Normal normal = intersection.facingNormal();

		Object::Radiance irradiance;

		for (const Object::Primitive &light : intersection.scene().areaLights()) {
			const Object::Radiance &objectRadiance = light.surface().radiance();
			const Object::Shape::Base::Sampler *shapeSampler = light.shape().sampler();
			if (!shapeSampler) {
				continue;
			}

			float surfaceArea = shapeSampler->surfaceArea();
			Math::Point2D surfacePoint = sampler.getValue2D();

			Math::Point samplePoint;
			Math::Vector du;
			Math::Vector dv;
			Math::Normal sampleNormal;

			shapeSampler->sample(surfacePoint, samplePoint, sampleNormal);

			Math::Vector incidentDirection = samplePoint - point;
			float distance = incidentDirection.magnitude();
			incidentDirection = incidentDirection / distance;
			localIncidentDirection = basis.worldToLocal(incidentDirection);

			float dot = incidentDirection * normal;
			if (dot > 0) {
				Math::Point offsetPoint = point + Math::Vector(normal) * 0.0001;
				Math::Ray ray(offsetPoint, incidentDirection);
				Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
				Object::Intersection intersection2 = scene.intersect(beam);

				Object::Radiance probeRadiance;
				if (intersection2.valid() && &(intersection2.primitive()) == &light) {
					irradiance = objectRadiance * dot / (distance * distance);
				}
			}

			break;
		}

		return irradiance;
	}
}
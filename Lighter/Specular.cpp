#define _USE_MATH_DEFINES
#include "Lighter/Specular.hpp"

#include "Render/Sampler.hpp"

#include <cmath>

namespace Lighter {
	Specular::Specular(const Lighter::Base &lighter, int maxGeneration, bool misDirect)
		: mLighter(lighter)
	{
		mMaxGeneration = maxGeneration;
		mMisDirect = misDirect;
	}

	Object::Radiance Specular::light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
	{
		const Object::Surface &surface = intersection.primitive().surface();
		const Math::Ray &ray = intersection.ray();
		const Math::Normal &normal = intersection.facingNormal();
		Math::Vector outgoingDirection = -ray.direction();

		Object::Radiance radiance;

		if (surface.brdf().hasSpecular() && generation < mMaxGeneration) {
			const Object::Scene &scene = intersection.scene();
			const Object::Brdf::Base &brdf = surface.brdf().specular();
			const Object::Color &albedo = intersection.albedo();

			Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01;

			Math::Point2D samplePoint = sampler.getValue2D();

			Math::Vector incidentDirection = brdf.sample(samplePoint, normal, outgoingDirection);
			float pdf = brdf.pdf(incidentDirection, normal, outgoingDirection);
			float dot = incidentDirection * normal;
			if(dot > 0) {
				Math::Ray reflectRay(offsetPoint, incidentDirection);
				Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
				Object::Intersection intersection2 = scene.intersect(beam);

				if (intersection2.valid()) {
					Object::Radiance incidentRadiance = mLighter.light(intersection2, sampler, generation + 1) * dot;
					Object::Radiance reflectedRadiance = brdf.reflected(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / pdf;
					float weight = 1.0f;
					if (mMisDirect && intersection2.primitive().surface().radiance().magnitude() > 0) {
						float directPdf = (1.0f / intersection2.primitive().shape().sampler()->surfaceArea()) * intersection2.distance() * intersection2.distance() / std::abs(intersection2.normal() * outgoingDirection);
						weight = pdf / (pdf + directPdf);
					}
					radiance += reflectedRadiance * weight;
				}
			}
		}

		return radiance;
	}
}

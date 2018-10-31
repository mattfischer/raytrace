#define _USE_MATH_DEFINES
#include "Lighter/Specular.hpp"

#include "Lighter/Sampler.hpp"

#include <cmath>

namespace Lighter {
	Specular::Specular(const Lighter::Base &lighter, int numSamples, int maxGeneration, bool misDirect, int numDirectSamples)
		: mLighter(lighter)
	{
		mNumSamples = numSamples;
		mMaxGeneration = maxGeneration;
		mMisDirect = misDirect;
		mNumDirectSamples = numDirectSamples;
	}

	Object::Radiance Specular::light(const Render::Intersection &intersection, Render::Tracer &tracer, int generation) const
	{
		const Object::Surface &surface = intersection.primitive().surface();
		const Math::Ray &ray = intersection.ray();
		Math::Normal normal = intersection.normal();
		Math::Vector outgoingDirection = -ray.direction();

		if (normal * outgoingDirection < 0) {
			normal = -normal;
		}

		Object::Radiance radiance;

		if (surface.brdf().hasSpecular() && generation < mMaxGeneration) {
			const Object::Brdf::Base &brdf = surface.brdf().specular();
			const Object::Color &albedo = intersection.albedo();

			Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01;

			Sampler sampler(mNumSamples, mRandomEngine);

			for (int i = 0; i < mNumSamples; i++) {
				Math::Point2D samplePoint = sampler.sample(i);

				Math::Vector incidentDirection = brdf.sample(samplePoint, normal, outgoingDirection);
				float pdf = brdf.pdf(incidentDirection, normal, outgoingDirection);
				float dot = incidentDirection * normal;
				if(dot > 0) {
					Math::Ray reflectRay(offsetPoint, incidentDirection);
					Render::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
					Render::Intersection intersection2 = tracer.intersect(beam);

					if (intersection2.valid()) {
						Object::Radiance incidentRadiance = mLighter.light(intersection2, tracer, generation + 1) * dot;
						Object::Radiance reflectedRadiance = brdf.reflected(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / (pdf * mNumSamples);
						float weight = 1.0f;
						if (mMisDirect && intersection2.primitive().surface().radiance().magnitude() > 0) {
							float directPdf = (1.0f / intersection2.primitive().shape().sampler()->surfaceArea()) * intersection2.distance() * intersection2.distance() / std::abs(intersection2.normal() * outgoingDirection);
							weight = (mNumSamples * pdf) / (mNumSamples * pdf + mNumDirectSamples * directPdf);
						}
						radiance += reflectedRadiance * weight;
					}
				}
			}
		}

		return radiance;
	}
}

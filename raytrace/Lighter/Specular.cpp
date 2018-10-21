#define _USE_MATH_DEFINES
#include "Lighter/Specular.hpp"

#include "Lighter/Sampler.hpp"

#include <cmath>

namespace Lighter {
	Specular::Specular(const Lighter::Base &lighter, int numSamples, int maxGeneration)
		: mLighter(lighter)
	{
		mNumSamples = numSamples;
		mMaxGeneration = maxGeneration;
	}

	Object::Radiance Specular::light(const Object::Intersection &intersection, Render::Tracer &tracer, int generation) const
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
				float u;
				float v;

				sampler.sample(i, u, v);

				float pdf;
				Math::Vector incidentDirection = brdf.sample(u, v, normal, outgoingDirection, pdf);
				float dot = incidentDirection * normal;
				if(dot > 0) {
					Math::Ray reflectRay(offsetPoint, incidentDirection);
					Object::Intersection intersection2 = tracer.intersect(reflectRay);

					if (intersection2.valid()) {
						Object::Radiance incidentRadiance = mLighter.light(intersection2, tracer, generation + 1) * dot;
						radiance += brdf.radiance(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / (pdf * mNumSamples);
					}
				}
			}
		}

		return radiance;
	}
}

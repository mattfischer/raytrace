#define _USE_MATH_DEFINES
#include "Lighter/Specular.hpp"

#include "Lighter/Utils.hpp"

#include "Render/Tracer.hpp"
#include "Object/Scene.hpp"

#include "Render/Engine.hpp"

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

		for (int i = 0; i < mNumSamples; i++) {
			float u;
			float v;

			Utils::stratifiedSamples(i, mNumSamples, u, v, mRandomEngine);

			Math::Vector incidentDirection = brdf.sample(u, v, normal, outgoingDirection);
			if(incidentDirection * normal > 0) {
				Math::Ray reflectRay(offsetPoint, incidentDirection);
				Object::Intersection intersection2 = tracer.intersect(reflectRay);

				if (intersection2.valid()) {
					Object::Radiance incidentRadiance = mLighter.light(intersection2, tracer, generation + 1);
					radiance += brdf.sampledRadiance(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo) / mNumSamples;
				}
			}
		}
	}

	return radiance;
}

}

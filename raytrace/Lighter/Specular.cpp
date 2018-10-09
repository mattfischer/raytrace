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
	const Object::Surface &surface = intersection.primitive()->surface();
	const Math::Ray &ray = intersection.ray();
	const Math::Normal &normal = intersection.normal();
	Object::Radiance radiance;

	Math::Vector x, y;
	Utils::orthonormalBasis(Math::Vector(normal), x, y);

	if (surface.brdf().specular() && generation < mMaxGeneration) {
		Object::Color albedo = surface.albedo().color(intersection.objectPoint());
		Math::Vector outgoingDirection = -ray.direction();
		Math::Point offsetPoint = intersection.point() + Math::Vector(intersection.normal()) * 0.01;

		for (int i = 0; i < mNumSamples; i++) {
			Math::Vector vector = Utils::sampleHemisphere(i, mNumSamples, M_PI / 2, mRandomEngine);

			Math::Vector incidentDirection = x * vector.x() + y * vector.y() + Math::Vector(normal) * vector.z();
			Math::Ray reflectRay(offsetPoint, incidentDirection);
			Object::Intersection intersection2 = tracer.intersect(reflectRay);

			if (intersection2.valid())
			{
				Object::Radiance reflectRadiance = mLighter.light(intersection2, tracer, generation + 1);
				radiance += surface.brdf().specularRadiance(reflectRadiance, incidentDirection, intersection.normal(), outgoingDirection, albedo) * 2 * M_PI / mNumSamples;
			}
		}
	}

	return radiance;
}

}

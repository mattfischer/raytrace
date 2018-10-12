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

	if (surface.brdf().hasSpecular() && generation < mMaxGeneration) {
		Object::Color albedo = surface.albedo().color(intersection.objectPoint());
		Math::Vector outgoingDirection = -ray.direction();
		Math::Point offsetPoint = intersection.point() + Math::Vector(intersection.normal()) * 0.01;

		Math::Vector x, y;
		Utils::orthonormalBasis(outgoingDirection, x, y);

		for (int i = 0; i < mNumSamples; i++) {
			float u;
			float v;

			Utils::stratifiedSamples(i, mNumSamples, u, v, mRandomEngine);

			float phi = 2 * M_PI * u;
			float theta = std::acos(std::pow(v, 1.0f / (101))); /****/

			Math::Vector direction = x * std::cos(phi) * std::sin(theta) + y * std::sin(phi) * std::sin(theta) + outgoingDirection * std::cos(theta);
			Math::Vector incidentDirection = -(direction - Math::Vector(normal) * (direction * normal * 2));
			if(incidentDirection * normal > 0) {
				Math::Ray reflectRay(offsetPoint, incidentDirection);
				Object::Intersection intersection2 = tracer.intersect(reflectRay);

				if (intersection2.valid()) {
					Object::Radiance reflectRadiance = mLighter.light(intersection2, tracer, generation + 1);
					radiance += reflectRadiance / mNumSamples;
				}
			}
		}
	}

	return radiance;
}

}

#define _USE_MATH_DEFINES
#include "Lighter/DiffuseIndirect.hpp"

#include "Object/Radiance.hpp"
#include "Object/Intersection.hpp"
#include "Object/Primitive/Base.hpp"
#include "Math/Ray.hpp"
#include "Render/Tracer.hpp"
#include "Lighter/Utils.hpp"

#include <cmath>
#include <algorithm>

namespace Lighter {

DiffuseIndirect::DiffuseIndirect(int indirectSamples, int indirectDirectSamples, bool irradianceCaching, float irradianceCacheThreshold)
	: mDirectLighter(indirectDirectSamples)
	, mIrradianceCache(irradianceCacheThreshold)
{
	mIndirectSamples = indirectSamples;
	mIrradianceCaching = irradianceCaching;
}

Object::Radiance DiffuseIndirect::light(const Object::Intersection &intersection, Render::Tracer &tracer) const
{
	const Math::Point &point = intersection.point();
	const Math::Normal &normal = intersection.normal();
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();

	clearProbe();

	Object::Radiance radiance;
	if (mIrradianceCaching) {
		Object::Radiance irradiance = mIrradianceCache.interpolateUnlocked(point, normal);
		radiance = irradiance * albedo * brdf.lambert() / M_PI;
	}
	else {
		Math::Vector x, y;
		Utils::orthonormalBasis(Math::Vector(intersection.normal()), x, y);
		const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
		const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();

		const int M = std::sqrt(mIndirectSamples);
		const int N = mIndirectSamples / M;
		for (int k = 0; k < N; k++) {
			for (int j = 0; j < M; j++) {
				std::uniform_real_distribution<float> dist(0, 1);

				float phi = 2 * M_PI * (k + dist(mRandomEngine)) / N;
				float theta = std::asin(std::sqrt((j + dist(mRandomEngine)) / M));
				Math::Vector direction = x * std::cos(phi) * std::cos(theta) + y * std::sin(phi) * std::cos(theta) + Math::Vector(intersection.normal()) * std::sin(theta);

				Math::Point offsetPoint = intersection.point() + Math::Vector(intersection.normal()) * 0.01;
				Math::Ray ray(offsetPoint, direction, intersection.ray().generation() + 1);
				Object::Intersection intersection2 = tracer.intersect(ray);

				Math::Vector probeDirection(std::cos(phi) * std::cos(theta), std::sin(phi) * std::cos(theta), std::sin(theta));
				Object::Radiance probeRadiance;
				if (intersection2.valid()) {
					Object::Radiance irradiance = mDirectLighter.light(intersection2, tracer);
					radiance += irradiance *  albedo * brdf.lambert() / (M * N);
					probeRadiance = irradiance;
				}

				addProbeEntry(probeDirection, probeRadiance);
			}
		}
	}

	return radiance;
}

bool DiffuseIndirect::prerender(const Object::Intersection &intersection, Render::Tracer &tracer)
{
	if (!mIrradianceCaching) {
		return false;
	}

	const Math::Point &point = intersection.point();
	const Math::Normal &normal = intersection.normal();

	if (mIrradianceCache.test(point, normal)) {
		return false;
	}

	Math::Vector x, y;
	Utils::orthonormalBasis(Math::Vector(normal), x, y);

	float mean = 0;
	int den = 0;
	Object::Radiance radiance;
	const int M = std::sqrt(mIndirectSamples);
	const int N = mIndirectSamples / M;
	std::vector<Object::Radiance> samples;
	std::vector<float> sampleDistances;
	samples.resize(M * N);
	sampleDistances.resize(M * N);
	for(int k = 0; k < N; k++) {
		for (int j = 0; j < M; j++) {
			std::uniform_real_distribution<float> dist(0, 1);

			float phi = 2 * M_PI * (k + dist(mRandomEngine)) / N;
			float theta = std::asin(std::sqrt((j + dist(mRandomEngine)) / M));
			Math::Vector direction = x * std::cos(phi) * std::cos(theta) + y * std::sin(phi) * std::cos(theta) + Math::Vector(normal) * std::sin(theta);

			Math::Point offsetPoint = point + Math::Vector(normal) * 0.01;
			Math::Ray ray(offsetPoint, direction, intersection.ray().generation() + 1);
			Object::Intersection intersection2 = tracer.intersect(ray);

			if (intersection2.valid()) {
				mean += 1 / intersection2.distance();
				den++;
				Object::Radiance incidentRadiance = mDirectLighter.light(intersection2, tracer);

				samples[k * M + j] = incidentRadiance;
				sampleDistances[k * M + j] = intersection2.distance();

				radiance += incidentRadiance * M_PI / (M * N);
			}
			else {
				sampleDistances[k * M + j] = FLT_MAX;
			}
		}
	}

	if (mean > 0) {
		mean = den / mean;

		IrradianceCache::Entry newEntry;
		newEntry.point = point;
		newEntry.normal = normal;
		newEntry.radiance = radiance;

		float radius = mean;
		float minRadius = 3 * tracer.projectedPixelSize(intersection.distance()) / mIrradianceCache.threshold();
		float maxRadius = 20 * minRadius;
		newEntry.radius = std::min(std::max(radius, minRadius), maxRadius);

		RadianceGradient transGrad;
		RadianceGradient rotGrad;
		for (int k = 0; k < N; k++) {
			int k1 = (k > 0) ? (k - 1) : N - 1;
			float phi = 2 * M_PI * k / N;
			Math::Vector u = x * std::cos(phi) + y * std::sin(phi);
			Math::Vector v = x * std::cos(phi + M_PI / 2) + y * std::sin(phi + M_PI / 2);

			for (int j = 0; j < M; j++) {
				float thetaMinus = std::asin(std::sqrt((float)j / M));
				float thetaPlus = std::asin(std::sqrt((float)(j + 1) / M));

				if (j > 0) {
					int j1 = j - 1;

					Math::Vector c = u * std::sin(thetaMinus) * std::cos(thetaMinus) * std::cos(thetaMinus) * 2 * M_PI / (N * std::min(sampleDistances[k * M + j], sampleDistances[k * M + j1]));
					transGrad += RadianceGradient(samples[k * M + j] - samples[k * M + j1], c);
				}

				Math::Vector c = v * (std::sin(thetaPlus) - std::sin(thetaMinus)) / std::min(sampleDistances[k * M + j], sampleDistances[k1 * M + j]);
				transGrad += RadianceGradient(samples[k * M + j] - samples[k1 * M + j], c);

				rotGrad += RadianceGradient(samples[k * M + j], v) * std::tan(thetaMinus) * M_PI / (M * N);
			}
		}

		if (radius < minRadius) {
			transGrad = transGrad * radius / minRadius;
		}

		newEntry.transGrad = transGrad;
		newEntry.rotGrad = rotGrad;

		mIrradianceCache.add(newEntry);
	}

	return true;
}

}
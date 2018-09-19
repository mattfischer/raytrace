#define _USE_MATH_DEFINES
#include "Trace/Lighter/Indirect.hpp"

#include "Object/Radiance.hpp"
#include "Trace/Intersection.hpp"
#include "Object/Primitive/Base.hpp"
#include "Trace/Ray.hpp"
#include "Trace/Tracer.hpp"
#include "Trace/Lighter/Utils.hpp"

#include <cmath>
#include <algorithm>

namespace Trace {
namespace Lighter {

Indirect::Indirect(int indirectSamples, int indirectDirectSamples)
	: mDirectLighter(indirectDirectSamples)
{
	mIndirectSamples = indirectSamples;
}

Object::Radiance Indirect::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Point &point = intersection.point();
	const Math::Normal &normal = intersection.normal();
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();

	Object::Radiance incidentRadiance;
	if (tracer.settings().irradianceCaching) {
		Trace::IrradianceCache &irradianceCache = tracer.renderData().irradianceCache;

		std::vector<IrradianceCache::Entry> entries = irradianceCache.lookup(point, normal);
		if (entries.size() > 0) {
			float den = 0;
			for (const IrradianceCache::Entry &entry : entries) {
				float weight = irradianceCache.weight(entry, point, normal);
				if (std::isinf(weight)) {
					incidentRadiance = entry.radiance;
					break;
				}
				Math::Vector cross = Math::Vector(normal % entry.normal);
				Math::Vector dist = point - entry.point;
				incidentRadiance += (entry.radiance + entry.rotGrad * cross + entry.transGrad * dist) * weight;
				den += weight;
			}
			incidentRadiance = incidentRadiance / den;
		}
	}
	else {
		incidentRadiance = sampleHemisphere(intersection, tracer);
	}

	return incidentRadiance * albedo * brdf.lambert() / M_PI;
}

Object::Radiance Indirect::sampleHemisphere(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	Math::Vector x, y;
	Utils::orthonormalBasis(Math::Vector(intersection.normal()), x, y);

	const int M = std::sqrt(mIndirectSamples);
	const int N = mIndirectSamples / M;
	Object::Radiance incidentRadiance;
	for (int k = 0; k < N; k++) {
		for (int j = 0; j < M; j++) {
			std::uniform_real_distribution<float> dist(0, 1);

			float phi = 2 * M_PI * (k + dist(mRandomEngine)) / N;
			float theta = std::asin(std::sqrt((j + dist(mRandomEngine)) / M));
			Math::Vector direction = x * std::cos(phi) * std::cos(theta) + y * std::sin(phi) * std::cos(theta) + Math::Vector(intersection.normal()) * std::sin(theta);

			Trace::Ray ray(intersection.point(), direction, intersection.ray().generation() + 1);
			Trace::IntersectionVector::iterator begin, end;
			tracer.intersect(ray, begin, end);

			if (begin != end) {
				Trace::Intersection intersection2 = *begin;
				Object::Radiance radiance = mDirectLighter.light(intersection2, tracer);
				incidentRadiance += radiance * M_PI / (M * N);

				Math::Vector v(std::cos(phi) * std::cos(theta), std::sin(phi) * std::cos(theta), std::sin(theta));
				addProbeEntry(v, radiance);
			}
		}
	}

	return incidentRadiance;
}

bool Indirect::prerender(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	if (!tracer.settings().irradianceCaching) {
		return false;
	}

	const Math::Point &point = intersection.point();
	const Math::Normal &normal = intersection.normal();
	Trace::IrradianceCache &irradianceCache = tracer.renderData().irradianceCache;

	std::vector<IrradianceCache::Entry> entries = irradianceCache.lookup(point, normal);
	if (entries.size() > 0) {
		return false;
	}

	Math::Vector x, y;
	Utils::orthonormalBasis(Math::Vector(normal), x, y);

	float mean = 0;
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

			Trace::Ray ray(intersection.point(), direction, intersection.ray().generation() + 1);
			Trace::IntersectionVector::iterator begin, end;
			tracer.intersect(ray, begin, end);

			if (begin != end) {
				Trace::Intersection intersection2 = *begin;

				mean += 1 / intersection2.distance();
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
		mean = M * N / mean;

		IrradianceCache::Entry newEntry;
		newEntry.point = point;
		newEntry.normal = normal;
		newEntry.radiance = radiance;

		float radius = mean;
		float minRadius = 3 * tracer.projectedPixelSize(intersection.distance()) / tracer.settings().irradianceCacheThreshold;
		float maxRadius = 20 * minRadius;
		newEntry.radius = std::min(std::max(radius, minRadius), maxRadius);

		Trace::RadianceGradient transGrad;
		Trace::RadianceGradient rotGrad;
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

		irradianceCache.add(newEntry);
	}

	return true;
}

}
}
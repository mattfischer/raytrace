#define _USE_MATH_DEFINES
#include "Trace/Lighter/IndirectCached.hpp"

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

IndirectCached::IndirectCached(int indirectSamples, int indirectDirectSamples)
	: mDirectLighter(indirectDirectSamples)
{
	mIndirectSamples = indirectSamples;
}

Object::Radiance IndirectCached::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Point &point = intersection.point();
	const Math::Normal &normal = intersection.normal();
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();
	Trace::IrradianceCache &irradianceCache = tracer.renderData().irradianceCache;

	std::vector<IrradianceCache::Entry> entries = irradianceCache.lookup(point, normal);
	if (entries.size() > 0) {
		Object::Radiance incidentRadiance;
		float den = 0;
		for (const IrradianceCache::Entry &entry : entries) {
			float weight = irradianceCache.weight(entry, point, normal);
			if (std::isinf(weight)) {
				return entry.radiance * albedo * brdf.lambert() / M_PI;
			}
			Math::Vector cross = Math::Vector(normal % entry.normal);
			Object::Radiance rotGrad(entry.rotGradR * cross, entry.rotGradG * cross, entry.rotGradB * cross);
			Math::Vector dist = point - entry.point;
			Object::Radiance transGrad(entry.transGradR * dist, entry.transGradG * dist, entry.transGradB * dist);
			incidentRadiance += (entry.radiance + rotGrad + transGrad) * weight;
			den += weight;
		}
		incidentRadiance = incidentRadiance / den;
		return incidentRadiance * albedo * brdf.lambert() / M_PI;
	}

	return Object::Radiance();
}

bool IndirectCached::prerender(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
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
	Math::Vector rotGradR;
	Math::Vector rotGradG;
	Math::Vector rotGradB;
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
			Math::Vector localDirection(std::cos(phi) * std::cos(theta), std::sin(phi) * std::cos(theta), std::sin(theta));
			Math::Vector direction = x * localDirection.x() + y * localDirection.y() + Math::Vector(normal) * localDirection.z();

			Trace::Ray ray(intersection.point(), direction, intersection.ray().generation() + 1);
			Trace::IntersectionVector::iterator begin, end;
			tracer.intersect(ray, begin, end);

			Math::Vector localV(std::cos(phi + M_PI / 2), std::sin(phi + M_PI / 2), 0);
			Math::Vector v = x * localV.x() + y * localV.y();
			float tan = std::tan(theta);

			if (begin != end) {
				Trace::Intersection intersection2 = *begin;

				mean += 1 / intersection2.distance();
				Object::Radiance incidentRadiance = mDirectLighter.light(intersection2, tracer);

				samples[k * M + j] = incidentRadiance;
				sampleDistances[k * M + j] = std::max(intersection2.distance(), 0.1f);

				radiance += incidentRadiance * M_PI / (M * N);
				rotGradR = rotGradR - v * tan * incidentRadiance.red() * M_PI / (M * N);
				rotGradG = rotGradG - v * tan * incidentRadiance.green() * M_PI / (M * N);
				rotGradB = rotGradB - v * tan * incidentRadiance.blue() * M_PI / (M * N);
			}
			else {
				sampleDistances[k * M + j] = FLT_MAX;
			}
		}
	}

	if (mean > 0) {
		IrradianceCache::Entry newEntry;
		newEntry.point = point;
		newEntry.normal = normal;
		newEntry.radiance = radiance;

		float radius = M * N / mean;
		float minRadius = 3 * tracer.projectedPixelSize(intersection.distance()) / tracer.settings().irradianceCacheThreshold;
		float maxRadius = 20 * minRadius;
		newEntry.radius = std::min(std::max(radius, minRadius), maxRadius);

		newEntry.rotGradR = rotGradR;
		newEntry.rotGradG = rotGradG;
		newEntry.rotGradB = rotGradB;

		Math::Vector transGradR;
		Math::Vector transGradG;
		Math::Vector transGradB;
		for (int k = 0; k < N; k++) {
			int k1 = (k > 0) ? (k - 1) : N - 1;
			float phi = 2 * M_PI * k / N;
			Math::Vector localU(std::cos(phi), std::sin(phi), 0);
			Math::Vector u = x * localU.x() + y * localU.y();

			Math::Vector localV(std::cos(phi + M_PI / 2), std::sin(phi + M_PI / 2), 0);
			Math::Vector v = x * localV.x() + y * localV.y();

			for (int j = 1; j < M; j++) {
				int j1 = j - 1;
				float theta = std::asin(std::sqrt((float)j / M));
				Math::Vector c = u * std::sin(theta) * std::cos(theta) * std::cos(theta) * 2 * M_PI / (N * std::min(sampleDistances[k * M + j], sampleDistances[k * M + j1]));

				transGradR = transGradR + c * (samples[k * M + j].red() - samples[k * M + j1].red());
				transGradG = transGradG + c * (samples[k * M + j].green() - samples[k * M + j1].green());
				transGradB = transGradB + c * (samples[k * M + j].blue() - samples[k * M + j1].blue());
			}

			for (int j = 0; j < M; j++) {
				float thetaMinus = std::asin(std::sqrt((float)j / M));
				float thetaPlus = std::asin(std::sqrt((float)(j + 1) / M));
				Math::Vector c = v * (std::sin(thetaPlus) - std::sin(thetaMinus)) / std::min(sampleDistances[k * M + j], sampleDistances[k1 * M + j]);

				transGradR = transGradR + c * (samples[k * M + j].red() - samples[k1 * M + j].red());
				transGradG = transGradG + c * (samples[k * M + j].green() - samples[k1 * M + j].green());
				transGradB = transGradB + c * (samples[k * M + j].blue() - samples[k1 * M + j].blue());
			}
		}

		if (radius < minRadius) {
			transGradR = transGradR * radius / minRadius;
			transGradG = transGradG * radius / minRadius;
			transGradB = transGradB * radius / minRadius;
		}

		newEntry.transGradR = transGradR;
		newEntry.transGradG = transGradG;
		newEntry.transGradB = transGradB;

		irradianceCache.add(newEntry);
	}

	return true;
}

}
}
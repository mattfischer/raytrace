#define _USE_MATH_DEFINES
#include "Lighter/DiffuseIndirect.hpp"

#include "Math/OrthonormalBasis.hpp"

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

	Object::Radiance DiffuseIndirect::light(const Object::Intersection &intersection, Render::Tracer &tracer, int generation) const
	{
		if (!intersection.primitive().surface().brdf().hasDiffuse()) {
			return Object::Radiance();
		}

		const Math::Point &point = intersection.point();
		Math::Normal normal = intersection.normal();
		const Object::Color &albedo = intersection.albedo();
		const Object::Brdf::Base &brdf = intersection.primitive().surface().brdf().diffuse();
		const Math::Vector &outgoingDirection = -intersection.ray().direction();

		if (outgoingDirection * normal < 0) {
			normal = -normal;
		}

		clearProbe();

		Object::Radiance radiance;
		if (mIrradianceCaching) {
			Object::Radiance irradiance = mIrradianceCache.interpolateUnlocked(point, normal);
			radiance = irradiance * albedo * brdf.lambert() / M_PI;
		}
		else {
			Math::OrthonormalBasis basis(intersection.normal());

			const int M = std::sqrt(mIndirectSamples);
			const int N = mIndirectSamples / M;
			for (int k = 0; k < N; k++) {
				for (int j = 0; j < M; j++) {
					std::uniform_real_distribution<float> dist(0, 1);

					float phi = 2 * M_PI * (k + dist(mRandomEngine)) / N;
					float theta = std::asin(std::sqrt((j + dist(mRandomEngine)) / M));
					Math::Vector direction = basis.localToWorld(Math::Vector::fromPolar(phi, theta, 1));
					Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01;
					Math::Ray ray(offsetPoint, direction);
					Object::Intersection intersection2 = tracer.intersect(ray);

					Math::Vector probeDirection = Math::Vector::fromPolar(phi, theta, 1);
					Object::Radiance probeRadiance;
					if (intersection2.valid()) {
						Object::Radiance irradiance = mDirectLighter.light(intersection2, tracer, generation + 1);
						radiance += irradiance * albedo * brdf.lambert() / (M * N);
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

		if (!intersection.primitive().surface().brdf().hasDiffuse()) {
			return false;
		}

		const Math::Point &point = intersection.point();
		Math::Normal normal = intersection.normal();
		const Math::Vector outgoingDirection = -intersection.ray().direction();
		if (outgoingDirection * normal < 0) {
			normal = -normal;
		}

		if (mIrradianceCache.test(point, normal)) {
			return false;
		}

		Math::OrthonormalBasis basis(normal);

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
				Math::Vector direction = basis.localToWorld(Math::Vector::fromPolar(phi, theta, 1));

				Math::Point offsetPoint = point + Math::Vector(normal) * 0.01;
				Math::Ray ray(offsetPoint, direction);
				Object::Intersection intersection2 = tracer.intersect(ray);

				if (intersection2.valid()) {
					mean += 1 / intersection2.distance();
					den++;
					Object::Radiance incidentRadiance = mDirectLighter.light(intersection2, tracer, 1);

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
				Math::Vector u = basis.localToWorld(Math::Vector::fromPolar(phi, 0, 1));
				Math::Vector v = basis.localToWorld(Math::Vector::fromPolar(phi + M_PI / 2, 0, 1));

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
			return true;
		}

		return false;
	}
}
#define _USE_MATH_DEFINES
#include "Lighter/DiffuseIndirect.hpp"

#include "Object/Scene.hpp"

#include "Render/TileJob.hpp"

#include <cmath>
#include <algorithm>

namespace Lighter {
	DiffuseIndirect::DiffuseIndirect(int indirectSamples, bool irradianceCaching, float irradianceCacheThreshold)
		: mDirectLighter(false)
		, mIrradianceCache(irradianceCacheThreshold)
	{
		mIndirectSamples = indirectSamples;
		mIrradianceCaching = irradianceCaching;
	}

	Object::Radiance DiffuseIndirect::light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const
	{
		if (!intersection.primitive().surface().brdf().hasDiffuse()) {
			return Object::Radiance();
		}

		const Math::Point &point = intersection.point();
		const Math::Normal &normal = intersection.facingNormal();
		const Object::Color &albedo = intersection.albedo();
		const Object::Brdf::Base &brdf = intersection.primitive().surface().brdf().diffuse();
		const Math::Vector &outgoingDirection = -intersection.ray().direction();

		Object::Radiance radiance;
		if (mIrradianceCaching) {
			Object::Radiance irradiance = mIrradianceCache.interpolateUnlocked(point, normal);
			radiance = irradiance * albedo * brdf.lambert() / M_PI;
		}
		else {
			Math::OrthonormalBasis basis(intersection.facingNormal());

			for (int i = 0; i < mIndirectSamples; i++) {
				Math::Vector incomingDirection;
				Object::Radiance irradiance = sampleIrradiance(intersection, basis, sampler, incomingDirection);
				radiance += irradiance * albedo * brdf.lambert() / mIndirectSamples;
			}
		}

		return radiance;
	}

	Object::Radiance DiffuseIndirect::sampleIrradiance(const Object::Intersection &intersection, const Math::OrthonormalBasis &basis, Render::Sampler &sampler, Math::Vector &localIncidentDirection) const
	{
		const Math::Normal &normal = intersection.facingNormal();

		float phi = 2 * M_PI * sampler.getValue();
		float theta = std::asin(std::sqrt(sampler.getValue()));
		Math::Vector direction = basis.localToWorld(Math::Vector::fromPolar(phi, theta, 1));
		Math::Point offsetPoint = intersection.point() + Math::Vector(normal) * 0.01;
		Math::Ray ray(offsetPoint, direction);
		Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
		Object::Intersection intersection2 = intersection.scene().intersect(beam);

		localIncidentDirection = Math::Vector::fromPolar(phi, theta, 1);
		Object::Radiance irradiance;
		if (intersection2.valid()) {
			irradiance = mDirectLighter.light(intersection2, sampler, 0) * theta;
		}

		return irradiance;
	}

	void DiffuseIndirect::prerenderPixel(int x, int y, Render::Framebuffer &framebuffer, const Object::Scene &scene, Render::Sampler &sampler)
	{
		Object::Color pixelColor;
		sampler.startSequence();
		Math::Beam beam = scene.camera().createPixelBeam(Math::Point2D(x, y), framebuffer.width(), framebuffer.height(), Math::Point2D());

		Object::Intersection intersection = scene.intersect(beam);

		if (intersection.valid() && intersection.primitive().surface().brdf().hasDiffuse()) {
			const Math::Point &point = intersection.point();
			const Math::Normal &normal = intersection.facingNormal();

			if (!mIrradianceCache.test(point, normal)) {
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
				for (int k = 0; k < N; k++) {
					for (int j = 0; j < M; j++) {
						std::uniform_real_distribution<float> dist(0, 1);

						float phi = 2 * M_PI * (k + dist(mRandomEngine)) / N;
						float theta = std::asin(std::sqrt((j + dist(mRandomEngine)) / M));
						Math::Vector direction = basis.localToWorld(Math::Vector::fromPolar(phi, theta, 1));

						Math::Point offsetPoint = point + Math::Vector(normal) * 0.01;
						Math::Ray ray(offsetPoint, direction);
						Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
						Object::Intersection intersection2 = scene.intersect(beam);

						if (intersection2.valid()) {
							mean += 1 / intersection2.distance();
							den++;
							sampler.startSample();
							Object::Radiance incidentRadiance = mDirectLighter.light(intersection2, sampler, 1);

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
					float projectedPixelSize = scene.camera().projectSize(2.0f / framebuffer.width(), intersection.distance());
					float minRadius = 3 * projectedPixelSize / mIrradianceCache.threshold();
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
					pixelColor = Object::Color(1, 1, 1);
				}
			}
		}

		framebuffer.setPixel(x, y, pixelColor);
	}

	std::vector<std::unique_ptr<Render::Job>> DiffuseIndirect::createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer)
	{
		std::vector<std::unique_ptr<Render::Job>> jobs;

		auto func = [&](int x, int y, Render::Framebuffer &framebuffer, Render::Sampler &sampler) {
			prerenderPixel(x, y, framebuffer, scene, sampler);
		};

		if (mIrradianceCaching) {
			jobs.push_back(std::make_unique<Render::TileJob>(framebuffer, std::move(func)));
		}

		return jobs;
	}
}
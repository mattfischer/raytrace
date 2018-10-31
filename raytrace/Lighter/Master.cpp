#include "Lighter/Master.hpp"

#include "Lighter/Direct.hpp"
#include "Lighter/DiffuseIndirect.hpp"
#include "Lighter/Radiant.hpp"
#include "Lighter/Specular.hpp"
#include "Lighter/Transmit.hpp"

namespace Lighter {
	Master::Master(const Settings &settings)
	{
		if (settings.directLighting) {
			mLighters.push_back(std::make_unique<Lighter::Direct>(settings.directSamples, settings.specularLighting, settings.specularSamples));
		}

		if (settings.indirectLighting) {
			mLighters.push_back(std::make_unique<Lighter::DiffuseIndirect>(settings.indirectSamples, settings.indirectDirectSamples, settings.irradianceCaching, settings.irradianceCacheThreshold));
		}

		if (settings.radiantLighting) {
			mLighters.push_back(std::make_unique<Lighter::Radiant>());
		}

		if (settings.specularLighting) {
			mLighters.push_back(std::make_unique<Lighter::Specular>(*this, settings.specularSamples, settings.specularMaxGeneration, settings.directLighting, settings.directSamples));
		}

		mLighters.push_back(std::make_unique<Lighter::Transmit>(*this, settings.specularMaxGeneration));
	}

	Object::Radiance Master::light(const Render::Intersection &intersection, Render::Tracer &tracer, int generation) const
	{
		Object::Radiance radiance;
		for (const std::unique_ptr<Lighter::Base> &lighter : mLighters) {
			radiance += lighter->light(intersection, tracer, generation);
		}

		return radiance;
	}

	bool Master::prerender(const Render::Intersection &intersection, Render::Tracer &tracer) const
	{
		bool ret = false;
		for (const std::unique_ptr<Lighter::Base> &lighter : mLighters) {
			if (lighter->prerender(intersection, tracer)) {
				ret = true;
			}
		}

		return ret;
	}
}
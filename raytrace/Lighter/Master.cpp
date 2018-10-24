#include "Lighter/Master.hpp"

#include "Lighter/DiffuseDirect.hpp"
#include "Lighter/DiffuseIndirect.hpp"
#include "Lighter/Radiant.hpp"
#include "Lighter/Specular.hpp"
#include "Lighter/Transmit.hpp"

namespace Lighter {
	Master::Master(const Settings &settings)
	{
		if (settings.directLighting) {
			mLighters.push_back(std::make_unique<Lighter::DiffuseDirect>(settings.directSamples));
		}

		if (settings.indirectLighting) {
			mLighters.push_back(std::make_unique<Lighter::DiffuseIndirect>(settings.indirectSamples, settings.indirectDirectSamples, settings.irradianceCaching, settings.irradianceCacheThreshold));
		}

		if (settings.radiantLighting) {
			mLighters.push_back(std::make_unique<Lighter::Radiant>());
		}

		if (settings.specularLighting) {
			mLighters.push_back(std::make_unique<Lighter::Specular>(*this, settings.specularSamples, settings.specularMaxGeneration));
		}

		mLighters.push_back(std::make_unique<Lighter::Transmit>(*this, settings.specularMaxGeneration));
	}

	Object::Radiance Master::light(const Object::Intersection &intersection, Render::Tracer &tracer, int generation) const
	{
		Object::Radiance radiance;
		for (const std::unique_ptr<Lighter::Base> &lighter : mLighters) {
			radiance += lighter->light(intersection, tracer, generation);
		}

		return radiance;
	}

	bool Master::prerender(const Object::Intersection &intersection, Render::Tracer &tracer) const
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
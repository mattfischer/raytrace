#ifndef LIGHTER_MASTER_HPP
#define LIGHTER_MASTER_HPP

#include "Lighter/Base.hpp"

#include <memory>
#include <vector>

namespace Lighter {
	class Master : public Base
	{
	public:
		struct Settings {
			bool radiantLighting;
			bool specularLighting;
			int specularMaxGeneration;
			bool directLighting;
			bool indirectLighting;
			int indirectSamples;
			bool irradianceCaching;
			float irradianceCacheThreshold;
		};

		Master(const Settings &settings);

		virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;
		virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);

	private:
		std::vector<std::unique_ptr<Lighter::Base>> mLighters;
	};
}
#endif
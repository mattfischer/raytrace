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
			int specularSamples;
			int specularMaxGeneration;
			bool directLighting;
			int directSamples;
			bool indirectLighting;
			int indirectSamples;
			int indirectDirectSamples;
			bool irradianceCaching;
			float irradianceCacheThreshold;
		};

		Master(const Settings &settings);

		virtual Object::Radiance light(const Render::Intersection &intersection, Render::Tracer &tracer, int generation) const;
		virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(Render::Framebuffer &framebuffer);

	private:
		std::vector<std::unique_ptr<Lighter::Base>> mLighters;
	};
}
#endif
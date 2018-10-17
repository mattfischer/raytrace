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

		virtual Object::Radiance light(const Object::Intersection &intersection, Render::Tracer &tracer, int generation) const;
		virtual bool prerender(const Object::Intersection &intersection, Render::Tracer &tracer) const;

	private:
		std::vector<std::unique_ptr<Lighter::Base>> mLighters;
	};
}
#endif
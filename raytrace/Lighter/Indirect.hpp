#ifndef LIGHTER_INDIRECT_HPP
#define LIGHTER_INDIRECT_HPP

#include "Lighter/Base.hpp"
#include "Lighter/Direct.hpp"
#include "Lighter/IrradianceCache.hpp"

#include <random>

namespace Lighter {

	class Indirect : public Base
	{
	public:
		Indirect(int indirectSamples, int indirectDirectSamples, bool irradianceCaching, float irradianceCacheThreshold);

		virtual Object::Radiance light(const Object::Intersection &intersection, Render::Tracer &tracer, Probe *probe = 0) const;
		virtual bool prerender(const Object::Intersection &intersection, Render::Tracer &tracer);

	private:
		mutable std::default_random_engine mRandomEngine;
		int mIndirectSamples;
		bool mIrradianceCaching;
		Direct mDirectLighter;
		IrradianceCache mIrradianceCache;
	};

}

#endif
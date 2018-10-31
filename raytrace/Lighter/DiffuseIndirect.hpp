#ifndef LIGHTER_DIFFUSE_INDIRECT_HPP
#define LIGHTER_DIFFUSE_INDIRECT_HPP

#include "Lighter/Base.hpp"
#include "Lighter/Direct.hpp"
#include "Lighter/IrradianceCache.hpp"

#include <random>

namespace Lighter {
	class DiffuseIndirect : public Base
	{
	public:
		DiffuseIndirect(int indirectSamples, int indirectDirectSamples, bool irradianceCaching, float irradianceCacheThreshold);

		virtual Object::Radiance light(const Render::Intersection &intersection, Render::Tracer &tracer, int generation) const;
		virtual bool prerender(const Render::Intersection &intersection, Render::Tracer &tracer);

	private:
		mutable std::default_random_engine mRandomEngine;
		int mIndirectSamples;
		bool mIrradianceCaching;
		Direct mDirectLighter;
		IrradianceCache mIrradianceCache;
	};
}

#endif
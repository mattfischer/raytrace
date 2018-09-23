#ifndef TRACE_LIGHTER_INDIRECT_HPP
#define TRACE_LIGHTER_INDIRECT_HPP

#include "Trace/Lighter/Base.hpp"
#include "Trace/Lighter/Direct.hpp"
#include "Trace/Lighter/IrradianceCache.hpp"

#include <random>

namespace Trace {
	namespace Lighter {

		class Indirect : public Base
		{
		public:
			Indirect(int indirectSamples, int indirectDirectSamples, bool irradianceCaching, float irradianceCacheThreshold);

			virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;
			virtual bool prerender(const Trace::Intersection &intersection, Trace::Tracer &tracer);

			Object::Radiance sampleHemisphere(const Trace::Intersection &intersection, Trace::Tracer &tracer, std::vector<ProbeEntry> *probe) const;

		private:
			mutable std::default_random_engine mRandomEngine;
			int mIndirectSamples;
			bool mIrradianceCaching;
			Direct mDirectLighter;
			IrradianceCache mIrradianceCache;
		};

	}
}

#endif
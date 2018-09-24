#ifndef TRACE_LIGHTER_SPECULAR_HPP
#define TRACE_LIGHTER_SPECULAR_HPP

#include "Trace/Lighter/Base.hpp"

namespace Render {
	class Engine;
}

namespace Trace {
	namespace Lighter {

		class Specular : public Base
		{
		public:
			Specular(const Render::Engine &engine, int maxRayGeneration);

			virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer, Probe *probe = 0) const;

		private:
			const Render::Engine &mEngine;
			int mMaxRayGeneration;
		};

	}
}

#endif
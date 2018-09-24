#ifndef TRACE_LIGHTER_SPECULAR_HPP
#define TRACE_LIGHTER_SPECULAR_HPP

#include "Trace/Lighter/Base.hpp"
#include "Render/Engine.hpp"

namespace Trace {
	namespace Lighter {

		class Specular : public Base
		{
		public:
			Specular(const Render::Engine &engine, int maxRayGeneration);

			virtual Object::Radiance light(const Object::Intersection &intersection, Render::Tracer &tracer, Probe *probe = 0) const;

		private:
			const Render::Engine &mEngine;
			int mMaxRayGeneration;
		};

	}
}

#endif
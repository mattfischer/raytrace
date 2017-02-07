#ifndef TRACE_LIGHTER_SPECULAR_HPP
#define TRACE_LIGHTER_SPECULAR_HPP

#include "Trace/Lighter/Base.hpp"

namespace Trace {
	namespace Lighter {

		class Specular : public Base
		{
		public:
			virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;
		};

	}
}

#endif
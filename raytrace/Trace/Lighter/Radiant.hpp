#ifndef TRACE_LIGHTER_RADIANT_HPP
#define TRACE_LIGHTER_RADIANT_HPP

#include "Trace/Lighter/Base.hpp"

namespace Trace {
	namespace Lighter {

		class Radiant : public Base
		{
		public:
			virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;
		};

	}
}

#endif
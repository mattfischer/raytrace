#ifndef TRACE_LIGHTER_DIRECT_POINT_HPP
#define TRACE_LIGHTER_DIRECT_POINT_HPP

#include "Trace/Lighter/Base.hpp"

#include <random>

namespace Trace {
	namespace Lighter {

		class DirectPoint : public Base
		{
		public:
			virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;
		};

	}
}

#endif
#ifndef TRACE_LIGHTER_PATH_HPP
#define TRACE_LIGHTER_PATH_HPP

#include "Trace/Lighter/Base.hpp"
#include "Trace/Lighter/Direct.hpp"

#include <random>

namespace Trace {
	namespace Lighter {

		class Path : public Base
		{
		public:
			Path();
			virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;

		private:
			mutable std::default_random_engine mRandomEngine;
			Direct mDirectLighter;
		};

	}
}

#endif
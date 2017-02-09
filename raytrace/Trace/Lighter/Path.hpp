#ifndef TRACE_LIGHTER_PATH_HPP
#define TRACE_LIGHTER_PATH_HPP

#include "Trace/Lighter/Base.hpp"

#include <random>

namespace Trace {
	namespace Lighter {

		class Path : public Base
		{
		public:
			virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;

		private:
			void orthoBasis(const Math::Vector &n, Math::Vector &x, Math::Vector &y) const;
			void randomAngles(int i, int N, float &phi, float &r) const;

			mutable std::default_random_engine mRandomEngine;
		};

	}
}

#endif
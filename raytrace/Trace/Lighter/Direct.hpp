#ifndef TRACE_LIGHTER_DIRECT_HPP
#define TRACE_LIGHTER_DIRECT_HPP

#include "Trace/Lighter/Base.hpp"

#include <random>

namespace Trace {
namespace Lighter {

class Direct : public Base
{
public:
	Direct(int numSamples);
	virtual Object::Radiance light(const Object::Intersection &intersection, Trace::Tracer &tracer, Probe *probe = 0) const;

private:
	int mNumSamples;
	mutable std::default_random_engine mRandomEngine;
};

}
}

#endif
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
	virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;

	Object::Radiance sampleHemisphere(const Trace::Intersection &intersection, Trace::Tracer &tracer, std::vector<ProbeEntry> *probe) const;

private:
	int mNumSamples;
	mutable std::default_random_engine mRandomEngine;
};

}
}

#endif
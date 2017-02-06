#ifndef TRACE_LIGHTER_DIRECT_HPP
#define TRACE_LIGHTER_DIRECT_HPP

#include "Trace/Lighter/Base.hpp"

namespace Trace {
namespace Lighter {

class Direct : public Base
{
public:
	virtual void light(const Trace::Intersection &intersection, Trace::Tracer &tracer, Accumulator &accumulator) const;
};

}
}

#endif
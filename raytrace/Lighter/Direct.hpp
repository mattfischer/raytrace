#ifndef LIGHTER_DIRECT_HPP
#define LIGHTER_DIRECT_HPP

#include "Lighter/Base.hpp"

namespace Lighter {

class Direct : public Base
{
public:
	virtual void light(const Trace::Intersection &intersection, Trace::Tracer &tracer, Accumulator &accumulator) const;
};

}

#endif
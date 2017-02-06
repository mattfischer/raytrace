#ifndef TRACE_LIGHTER_BASE_HPP
#define TRACE_LIGHTER_BASE_HPP

#include "Trace/Forwards.hpp"
#include "Object/Forwards.hpp"
#include "Math/Forwards.hpp"

#include <vector>

namespace Trace {
namespace Lighter {

class Base;
typedef std::vector<const Base*> LighterVector;

class Base {
public:
	class Accumulator {
	public:
		virtual void accumulate(const Object::Color &color, const Math::Vector &direction) = 0;
	};

	virtual void light(const Trace::Intersection &intersection, Trace::Tracer &tracer, Accumulator &accumulator) const = 0;

	static LighterVector createLighters();
};

}
}

#endif
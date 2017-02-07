#ifndef TRACE_LIGHTER_BASE_HPP
#define TRACE_LIGHTER_BASE_HPP

#include "Trace/Forwards.hpp"
#include "Object/Forwards.hpp"
#include "Math/Forwards.hpp"

#include <vector>
#include <memory>

namespace Trace {
namespace Lighter {

class Base;

class Base {
public:
	class Accumulator {
	public:
		virtual void accumulate(const Object::Radiance &radiance, const Math::Vector &direction) = 0;
	};

	virtual void light(const Trace::Intersection &intersection, Trace::Tracer &tracer, Accumulator &accumulator) const = 0;

	static std::vector<std::unique_ptr<Base>> createLighters();
};

}
}

#endif
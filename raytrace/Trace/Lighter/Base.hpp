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
	virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const = 0;

	static std::vector<std::unique_ptr<Base>> createLighters();
};

}
}

#endif
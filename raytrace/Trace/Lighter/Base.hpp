#ifndef TRACE_LIGHTER_BASE_HPP
#define TRACE_LIGHTER_BASE_HPP

#include "Trace/Forwards.hpp"
#include "Object/Forwards.hpp"
#include "Math/Forwards.hpp"

#include <vector>
#include <memory>

#include "Math/Vector.hpp"
#include "Object/Radiance.hpp"

namespace Trace {
namespace Lighter {

class Base;

class Base {
public:
	Base();

	virtual Object::Radiance light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const = 0;
	virtual bool prerender(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;

	struct ProbeEntry {
		Math::Vector direction;
		Object::Radiance radiance;
	};
};

}
}

#endif
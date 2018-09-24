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

	struct Probe {
		struct Entry {
			Math::Vector direction;
			Object::Radiance radiance;
		};

		std::vector<Entry> entries;
	};

	virtual Object::Radiance light(const Object::Intersection &intersection, Trace::Tracer &tracer, Probe *probe = 0) const = 0;
	virtual bool prerender(const Object::Intersection &intersection, Trace::Tracer &tracer);
};

}
}

#endif
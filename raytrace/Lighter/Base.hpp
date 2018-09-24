#ifndef LIGHTER_BASE_HPP
#define LIGHTER_BASE_HPP

#include "Object/Forwards.hpp"
#include "Math/Forwards.hpp"

#include "Render/Tracer.hpp"

#include <vector>
#include <memory>

#include "Math/Vector.hpp"
#include "Object/Radiance.hpp"

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

	virtual Object::Radiance light(const Object::Intersection &intersection, Render::Tracer &tracer, Probe *probe = 0) const = 0;
	virtual bool prerender(const Object::Intersection &intersection, Render::Tracer &tracer);
};

}

#endif
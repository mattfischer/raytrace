#ifndef LIGHTER_RADIANT_HPP
#define LIGHTER_RADIANT_HPP

#include "Lighter/Base.hpp"

namespace Lighter {
	class Radiant : public Base
	{
	public:
		virtual Object::Radiance light(const Render::Intersection &intersection, Render::Tracer &tracer, int generation) const;
	};
}

#endif
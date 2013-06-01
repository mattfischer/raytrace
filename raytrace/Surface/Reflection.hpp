#ifndef SURFACE_REFLECTION_HPP
#define SURFACE_REFLECTION_HPP

#include "Surface/Base.hpp"

namespace Surface {

class Reflection : public Base
{
public:
	virtual Object::Color color(const Trace::Intersection &intersection, const Trace::Tracer &tracer) const;
};

}

#endif
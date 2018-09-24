#include "Lighter/Base.hpp"

namespace Lighter {

Base::Base()
{
}

bool Base::prerender(const Object::Intersection &intersection, Render::Tracer &tracer)
{
	return false;
}

}
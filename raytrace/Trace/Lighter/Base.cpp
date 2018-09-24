#include "Trace/Lighter/Base.hpp"

namespace Trace {
namespace Lighter {

Base::Base()
{
}

bool Base::prerender(const Object::Intersection &intersection, Trace::Tracer &tracer)
{
	return false;
}

}
}
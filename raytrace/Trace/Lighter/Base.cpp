#include "Trace/Lighter/Base.hpp"

namespace Trace {
namespace Lighter {

Base::Base()
{
}

bool Base::prerender(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	return false;
}

}
}
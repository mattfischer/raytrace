#include "Trace/Lighter/Radiant.hpp"

#include "Object/Radiance.hpp"
#include "Trace/Intersection.hpp"
#include "Object/Primitive/Base.hpp"

namespace Trace {
namespace Lighter {

Object::Radiance Radiant::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	return intersection.primitive()->surface().radiance();
}

}
}
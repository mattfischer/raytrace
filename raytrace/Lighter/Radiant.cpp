#include "Lighter/Radiant.hpp"

#include "Object/Radiance.hpp"
#include "Object/Intersection.hpp"
#include "Object/Primitive/Base.hpp"

namespace Lighter {

Object::Radiance Radiant::light(const Object::Intersection &intersection, Render::Tracer &tracer, Probe *probe) const
{
	return intersection.primitive()->surface().radiance();
}

}

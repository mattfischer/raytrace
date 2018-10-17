#include "Lighter/Radiant.hpp"

namespace Lighter {

Object::Radiance Radiant::light(const Object::Intersection &intersection, Render::Tracer &tracer, int generation) const
{
	return intersection.primitive().surface().radiance();
}

}

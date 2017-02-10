#define _USE_MATH_DEFINES
#include "Trace/Lighter/Path.hpp"

#include "Object/Radiance.hpp"
#include "Trace/Intersection.hpp"
#include "Object/Primitive/Base.hpp"
#include "Trace/Ray.hpp"
#include "Trace/Tracer.hpp"
#include "Trace/Lighter/Utils.hpp"

#include <cmath>

namespace Trace {
namespace Lighter {

Path::Path()
	: mDirectLighter(10)
{
}

Object::Radiance Path::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Vector normal(intersection.normal());
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();

	Math::Vector x, y;
	Utils::orthonormalBasis(normal, x, y);

	Object::Radiance radiance;
	const int N = 25;
	for (int i = 0; i < N; i++) {
		Math::Vector v = Utils::sampleHemisphereCosineWeighted(i, N, mRandomEngine);
		Math::Vector incidentDirection = x * v.x() + y * v.y() + normal * v.z();

		Trace::Ray ray(intersection.point(), incidentDirection, intersection.ray().generation() + 1);
		Trace::IntersectionVector::iterator begin, end;
		tracer.intersect(ray, begin, end);

		if (begin != end) {
			Trace::Intersection intersection2 = *begin;
			Object::Radiance incidentRadiance = mDirectLighter.light(intersection2, tracer);
			radiance += brdf.radiance(incidentRadiance, incidentDirection, intersection.normal(), outgoingDirection, albedo);
		}
	}

	radiance = radiance * 2 * M_PI / N;

	return radiance;
}

}
}
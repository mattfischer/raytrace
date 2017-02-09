#define _USE_MATH_DEFINES
#include "Trace/Lighter/Path.hpp"

#include "Object/Radiance.hpp"
#include "Trace/Intersection.hpp"
#include "Object/Primitive/Base.hpp"
#include "Trace/Ray.hpp"
#include "Trace/Tracer.hpp"

#include <cmath>

namespace Trace {
namespace Lighter {

void orthoBasis(const Math::Vector &n, Math::Vector &x, Math::Vector &y)
{
	x = Math::Vector();
	y = Math::Vector();

	Math::Vector vectors[] = { Math::Vector(1,0,0), Math::Vector(0,1,0), Math::Vector(0,0,1) };

	for (const Math::Vector &v : vectors) {
		Math::Vector p = v - n * (v * n);
		if (p.magnitude2() > x.magnitude2()) {
			y = x;
			x = p;
		}
		else if (p.magnitude2() > y.magnitude2()) {
			y = p;
		}
	}

	x = x.normalize();
	y = y - x * (x * y);

	y = y.normalize();
}

Object::Radiance Path::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Vector normal(intersection.normal());
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();

	Math::Vector x, y;
	orthoBasis(normal, x, y);

	std::uniform_real_distribution<float> d(0, 1);

	Object::Radiance radiance;
	const int N = 10;
	for (int i = 0; i < N; i++) {
		float phi = d(mRandomEngine) * 2 * M_PI;
		float u = d(mRandomEngine);
		float r = std::sqrt(u);
		Math::Vector incidentDirection = x * r * cos(phi) + y * r * sin(phi) + normal * std::sqrt(1 - u);

		Trace::Ray ray(intersection.point(), incidentDirection, intersection.ray().generation() + 1);
		Trace::IntersectionVector::iterator begin, end;
		tracer.intersect(ray, begin, end);

		if (begin != end) {
			const Object::Radiance &incidentRadiance = begin->primitive()->surface().radiance();
			radiance += brdf.radiance(incidentRadiance, incidentDirection, intersection.normal(), outgoingDirection, albedo);
		}
	}

	radiance = radiance * 2 * M_PI / N;

	return radiance;
}

}
}
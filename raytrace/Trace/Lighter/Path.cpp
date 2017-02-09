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

void Path::orthoBasis(const Math::Vector &n, Math::Vector &x, Math::Vector &y) const
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

void Path::randomAngles(int i, int rootN, float &phi, float &r) const
{
	int a = i / rootN;
	int b = i % rootN;
	std::uniform_real_distribution<float> d(0, 1);
	float u = (a + d(mRandomEngine)) / rootN;
	float v = (b + d(mRandomEngine)) / rootN;
	phi = v * 2 * M_PI;
	r = std::sqrt(u);
}

Object::Radiance Path::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Vector normal(intersection.normal());
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();

	Math::Vector x, y;
	orthoBasis(normal, x, y);

	Object::Radiance radiance;
	const int rootN = 5 / intersection.ray().generation();
	for (int i = 0; i < rootN * rootN; i++) {
		float phi, r;
		randomAngles(i, rootN, phi, r);

		Math::Vector incidentDirection = x * r * cos(phi) + y * r * sin(phi) + normal * std::sqrt(1 - r * r);

		Trace::Ray ray(intersection.point(), incidentDirection, intersection.ray().generation() + 1);
		Trace::IntersectionVector::iterator begin, end;
		tracer.intersect(ray, begin, end);

		if (begin != end) {
			Trace::Intersection intersection2 = *begin;
			Object::Radiance incidentRadiance = intersection2.primitive()->surface().radiance();
			if (ray.generation() < 3) {
				incidentRadiance += light(intersection2, tracer);
			}
			radiance += brdf.radiance(incidentRadiance, incidentDirection, intersection.normal(), outgoingDirection, albedo);
		}
	}

	radiance = radiance * 2 * M_PI / (rootN * rootN);

	return radiance;
}

}
}
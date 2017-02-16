#define _USE_MATH_DEFINES
#include "Trace/Lighter/Indirect.hpp"

#include "Object/Radiance.hpp"
#include "Trace/Intersection.hpp"
#include "Object/Primitive/Base.hpp"
#include "Trace/Ray.hpp"
#include "Trace/Tracer.hpp"
#include "Trace/Lighter/Utils.hpp"

#include <cmath>
#include <algorithm>

namespace Trace {
namespace Lighter {

Indirect::Indirect(int indirectSamples, int indirectDirectSamples)
	: mDirectLighter(indirectDirectSamples)
{
	mIndirectSamples = indirectSamples;
}

Object::Radiance Indirect::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Point &point = intersection.point();
	const Math::Normal &normal = intersection.normal();
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();
	Trace::IrradianceCache &irradianceCache = tracer.renderData().irradianceCache;

	std::vector<IrradianceCache::Entry> entries = irradianceCache.lookup(point, normal);
	if (entries.size() > 0) {
		Object::Radiance radiance;
		float den = 0;
		for (const IrradianceCache::Entry &entry : entries) {
			float weight = irradianceCache.weight(entry, point, normal);
			if (std::isinf(weight)) {
				return radiance * albedo;
			}
			Math::Vector cross = Math::Vector(normal % entry.normal);
			Object::Radiance rotRad(entry.rotGradR * cross, entry.rotGradG * cross, entry.rotGradB * cross);
			radiance += (entry.radiance + rotRad ) * weight;
			den += weight;
		}
		return radiance * albedo / den;
	}

	return Object::Radiance();
}

bool Indirect::prerender(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Point point(intersection.point());
	const Math::Vector normal(intersection.normal());
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();
	Trace::IrradianceCache &irradianceCache = tracer.renderData().irradianceCache;

	std::vector<IrradianceCache::Entry> entries = irradianceCache.lookup(point, intersection.normal());
	if (entries.size() > 0) {
		return false;
	}

	Math::Vector x, y;
	Utils::orthonormalBasis(normal, x, y);

	float mean = 0;
	Object::Radiance cacheRadiance;
	Object::Radiance radiance;
	Math::Vector rotGradR;
	Math::Vector rotGradG;
	Math::Vector rotGradB;
	for (int i = 0; i < mIndirectSamples; i++) {
		Math::Vector v = Utils::sampleHemisphereCosineWeighted(i, mIndirectSamples, mRandomEngine);
		Math::Vector incidentDirection = x * v.x() + y * v.y() + normal * v.z();

		Trace::Ray ray(intersection.point(), incidentDirection, intersection.ray().generation() + 1);
		Trace::IntersectionVector::iterator begin, end;
		tracer.intersect(ray, begin, end);

		Math::Vector plane(v.x(), v.y(), 0);
		float r = plane.magnitude();
		plane = plane / r;
		float tan = incidentDirection.z() / r;
		Math::Vector perp(-plane.y(), plane.x(), 0);
		Math::Vector perpTrans = x * perp.x() + y * perp.y();

		if (begin != end) {
			Trace::Intersection intersection2 = *begin;

			mean += 1 / intersection2.distance();
			Object::Radiance incidentRadiance = mDirectLighter.light(intersection2, tracer);
			Object::Radiance c = incidentRadiance * brdf.lambert();
			cacheRadiance += c / (2 * M_PI);

			rotGradR = rotGradR + perpTrans * tan * c.red();
			rotGradG = rotGradG + perpTrans * tan * c.green();
			rotGradB = rotGradB + perpTrans * tan * c.blue();
			radiance += brdf.radiance(incidentRadiance, incidentDirection, intersection.normal(), outgoingDirection, albedo);
		}
	}

	radiance = radiance * 2 * M_PI / mIndirectSamples;
	rotGradR = rotGradR * M_PI / mIndirectSamples;
	rotGradG = rotGradG * M_PI / mIndirectSamples;
	rotGradB = rotGradB * M_PI / mIndirectSamples;

	if (mean > 0) {
		IrradianceCache::Entry newEntry;
		newEntry.point = point;
		newEntry.normal = intersection.normal();
		newEntry.radius = mIndirectSamples / mean;
		newEntry.radiance = cacheRadiance * 2 * M_PI / mIndirectSamples;
		newEntry.rotGradR = rotGradR;
		newEntry.rotGradG = rotGradG;
		newEntry.rotGradB = rotGradB;

		irradianceCache.add(newEntry);
	}

	return true;
}

}
}
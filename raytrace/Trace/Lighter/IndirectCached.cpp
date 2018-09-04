#define _USE_MATH_DEFINES
#include "Trace/Lighter/IndirectCached.hpp"

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

IndirectCached::IndirectCached(int indirectSamples, int indirectDirectSamples)
	: mDirectLighter(indirectDirectSamples)
{
	mIndirectSamples = indirectSamples;
}

Object::Radiance IndirectCached::light(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Point &point = intersection.point();
	const Math::Normal &normal = intersection.normal();
	const Object::Color &albedo = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
	const Object::Brdf::Base &brdf = intersection.primitive()->surface().brdf();
	const Math::Vector &outgoingDirection = -intersection.ray().direction();
	Trace::IrradianceCache &irradianceCache = tracer.renderData().irradianceCache;

	std::vector<IrradianceCache::Entry> entries = irradianceCache.lookup(point, normal);
	if (entries.size() > 0) {
		Object::Radiance incidentRadiance;
		float den = 0;
		for (const IrradianceCache::Entry &entry : entries) {
			float weight = irradianceCache.weight(entry, point, normal);
			if (std::isinf(weight)) {
				return entry.radiance * albedo * brdf.lambert() / M_PI;
			}
			Math::Vector cross = Math::Vector(normal % entry.normal);
			Object::Radiance rotRad(entry.rotGradR * cross, entry.rotGradG * cross, entry.rotGradB * cross);
			incidentRadiance += (entry.radiance + rotRad ) * weight;
			den += weight;
		}
		incidentRadiance = incidentRadiance / den;
		return incidentRadiance * albedo * brdf.lambert() / M_PI;
	}

	return Object::Radiance();
}

bool IndirectCached::prerender(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	const Math::Point &point = intersection.point();
	const Math::Normal &normal = intersection.normal();
	Trace::IrradianceCache &irradianceCache = tracer.renderData().irradianceCache;

	std::vector<IrradianceCache::Entry> entries = irradianceCache.lookup(point, normal);
	if (entries.size() > 0) {
		return false;
	}

	Math::Vector x, y;
	Utils::orthonormalBasis(Math::Vector(normal), x, y);

	float mean = 0;
	Object::Radiance radiance;
	Math::Vector rotGradR;
	Math::Vector rotGradG;
	Math::Vector rotGradB;
	for (int i = 0; i < mIndirectSamples; i++) {
		Math::Vector v = Utils::sampleHemisphereCosineWeighted(i, mIndirectSamples, mRandomEngine);
		Math::Vector incidentDirection = x * v.x() + y * v.y() + Math::Vector(normal) * v.z();

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
			Object::Radiance incidentRadiance = mDirectLighter.light(intersection2, tracer) * 2 * M_PI / mIndirectSamples;

			radiance += incidentRadiance;
			rotGradR = rotGradR + perpTrans * tan * incidentRadiance.red();
			rotGradG = rotGradG + perpTrans * tan * incidentRadiance.green();
			rotGradB = rotGradB + perpTrans * tan * incidentRadiance.blue();
		}
	}

	if (mean > 0) {
		IrradianceCache::Entry newEntry;
		newEntry.point = point;
		newEntry.normal = normal;
		newEntry.radius = std::max(std::min(mIndirectSamples / mean, 10.0f), 1.0f);
		newEntry.radiance = radiance;
		newEntry.rotGradR = rotGradR;
		newEntry.rotGradG = rotGradG;
		newEntry.rotGradB = rotGradB;

		irradianceCache.add(newEntry);
	}

	return true;
}

}
}
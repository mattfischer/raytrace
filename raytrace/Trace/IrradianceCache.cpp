#define _USE_MATH_DEFINES
#include "Trace/IrradianceCache.hpp"

#include "Math/Vector.hpp"

#include <algorithm>

namespace Trace {

float IrradianceCache::weight(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const 
{
	//return std::pow(std::max(double(0), 1.0f - (point - entry.point).magnitude2() / (1.0 * entry.radius * std::pow(normal * entry.normal, 4.0f))), 2);
	return 1.0f / ((point - entry.point).magnitude() / entry.radius + std::sqrt(1 - normal * entry.normal));
}

float IrradianceCache::error(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const
{
	return ((4.0f / M_PI) * (point - entry.point).magnitude() / entry.radius + std::sqrt(1 - normal * entry.normal));
}

std::vector<IrradianceCache::Entry> IrradianceCache::lookup(const Math::Point &point, const Math::Normal &normal) const
{
	std::vector<Entry> entries;
	std::lock_guard<std::mutex> guard(mMutex);
	float a = 2.0f;

	for (const Entry &e : mEntries)
	{
		float d = (point - e.point) * ((normal + e.normal) / 2);
		if (d >= -0.01 && error(e, point, normal) < 1 / a)
		{
			entries.push_back(e);
		}
	}

	return entries;
}

void IrradianceCache::add(const Entry &entry)
{
	std::lock_guard<std::mutex> guard(mMutex);
	mEntries.push_back(entry);
}

void IrradianceCache::clear()
{
	mEntries.clear();
}

}
#ifndef TRACE_IRRADIANCE_CACHE_HPP
#define TRACE_IRRADIANCE_CACHE_HPP

#include "Math/Point.hpp"
#include "Math/Normal.hpp"
#include "Object/Radiance.hpp"

#include <vector>
#include <mutex>

namespace Trace {

class IrradianceCache
{
public:
	struct Entry {
		Math::Point point;
		Math::Normal normal;
		float radius;
		Object::Radiance radiance;
	};

	float weight(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const;
	std::vector<Entry> lookup(const Math::Point &point, const Math::Normal &normal) const;
	void add(const Entry &entry);
	void clear();

private:
	std::vector<Entry> mEntries;
	mutable std::mutex mMutex;
};

}
#endif
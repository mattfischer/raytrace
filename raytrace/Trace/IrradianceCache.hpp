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

	IrradianceCache();
	float weight(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const;
	float error(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const;
	std::vector<Entry> lookup(const Math::Point &point, const Math::Normal &normal) const;
	void add(const Entry &entry);
	void clear();

private:
	struct OctreeNode
	{
		std::vector<Entry> entries;
		std::unique_ptr<OctreeNode> children[8];
	};

	void lookupOctreeNode(OctreeNode *node, Math::Point origin, float size, const Math::Point &point, const Math::Normal &normal, std::vector<Entry> &entries) const;

	std::unique_ptr<OctreeNode> mOctree;
	Math::Point mOctreeOrigin;
	float mOctreeSize;
	mutable std::mutex mMutex;
};

}
#endif
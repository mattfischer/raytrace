#ifndef LIGHTER_IRRADIANCE_CACHE_HPP
#define LIGHTER_IRRADIANCE_CACHE_HPP

#include "Math/Point.hpp"
#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Object/Radiance.hpp"
#include "Lighter/RadianceGradient.hpp"

#include <vector>
#include <mutex>

namespace Lighter {
	class IrradianceCache
	{
	public:
		struct Entry {
			Math::Point point;
			Math::Normal normal;
			float radius;
			Object::Radiance radiance;
			RadianceGradient rotGrad;
			RadianceGradient transGrad;
		};

		IrradianceCache(float threshold);
		float threshold() const;

		float weight(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const;
		float error(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const;
		bool test(const Math::Point &point, const Math::Normal &normal) const;
		bool testUnlocked(const Math::Point &point, const Math::Normal &normal) const;
		Object::Radiance interpolate(const Math::Point &point, const Math::Normal &normal) const;
		Object::Radiance interpolateUnlocked(const Math::Point &point, const Math::Normal &normal) const;
		void add(const Entry &entry);
		void clear();

	private:
		struct OctreeNode
		{
			std::vector<Entry> entries;
			std::unique_ptr<OctreeNode> children[8];
		};

		bool testOctreeNode(OctreeNode *node, const Math::Point &origin, float size, const Math::Point &point, const Math::Normal &normal) const;
		void interpolateOctreeNode(OctreeNode *node, const Math::Point &origin, float size, const Math::Point &point, const Math::Normal &normal, float threshold, Object::Radiance &radiance, float &totalWeight) const;

		float distance2ToNode(const Math::Point &point, int idx, const Math::Point &origin, float size) const;
		void getChildNode(const Math::Point &origin, float size, int idx, Math::Point &childOrigin, float &childSize) const;
		bool isEntryValid(const Entry &entry, const Math::Point &point, const Math::Normal &normal, float threshold) const;

		std::unique_ptr<OctreeNode> mOctreeRoot;
		Math::Point mOctreeOrigin;
		float mOctreeSize;
		mutable std::mutex mMutex;
		float mThreshold;
	};
}

#endif
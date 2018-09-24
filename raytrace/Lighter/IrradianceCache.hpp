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
		std::vector<Entry> lookup(const Math::Point &point, const Math::Normal &normal) const;
		std::vector<Entry> lookupUnlocked(const Math::Point &point, const Math::Normal &normal) const;
		void add(const Entry &entry);
		void clear();

	private:
		struct OctreeNode
		{
			std::vector<Entry> entries;
			std::unique_ptr<OctreeNode> children[8];
		};

		void lookupOctreeNode(OctreeNode *node, Math::Point origin, float size, const Math::Point &point, const Math::Normal &normal, std::vector<Entry> &entries) const;

		std::unique_ptr<OctreeNode> mOctreeRoot;
		Math::Point mOctreeOrigin;
		float mOctreeSize;
		mutable std::mutex mMutex;
		float mThreshold;
	};

}

#endif
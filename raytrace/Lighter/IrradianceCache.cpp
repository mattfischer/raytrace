#define _USE_MATH_DEFINES
#include "Lighter/IrradianceCache.hpp"

#include "Math/Vector.hpp"

#include <algorithm>

namespace Lighter {
	IrradianceCache::IrradianceCache(float threshold)
	{
		mOctreeSize = 0;
		mThreshold = threshold;
	}

	float IrradianceCache::threshold() const
	{
		return mThreshold;
	}

	float IrradianceCache::weight(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const 
	{
		//return std::pow(std::max(double(0), 1.0f - (point - entry.point).magnitude2() / (1.0 * entry.radius * std::pow(normal * entry.normal, 4.0f))), 2);
		return 1.0f / ((point - entry.point).magnitude() / entry.radius + std::sqrt(1 - std::min(1.0f, normal * entry.normal)));
	}

	float IrradianceCache::error(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const
	{
		return ((4.0f / M_PI) * (point - entry.point).magnitude() / entry.radius + std::sqrt(1 - normal * entry.normal));
	}

	float IrradianceCache::distance2ToNode(const Math::Point &point, int idx, const Math::Point &origin, float size) const
	{
		float distance2 = 0;
		float d;

		if (point.x() < origin.x() - size) { d = ((origin.x() - size) - point.x()); distance2 += d * d; }
		if (point.y() < origin.y() - size) { d = ((origin.y() - size) - point.y()); distance2 += d * d; }
		if (point.z() < origin.z() - size) { d = ((origin.z() - size) - point.z()); distance2 += d * d; }
		if (point.x() > origin.x() + size) { d = (point.x() - (origin.x() + size)); distance2 += d * d; }
		if (point.y() > origin.y() + size) { d = (point.y() - (origin.y() + size)); distance2 += d * d; }
		if (point.z() > origin.z() + size) { d = (point.z() - (origin.z() + size)); distance2 += d * d; }

		return distance2;
	}

	void IrradianceCache::getChildNode(const Math::Point &origin, float size, int idx, Math::Point &childOrigin, float &childSize) const
	{
		float x = (idx & 1) ? 1 : -1;
		float y = (idx & 2) ? 1 : -1;
		float z = (idx & 4) ? 1 : -1;

		childSize = size / 2;
		childOrigin = origin + Math::Vector(x, y, z) * childSize;
	}

	bool IrradianceCache::isEntryValid(const Entry &entry, const Math::Point &point, const Math::Normal &normal, float threshold) const
	{
		float d = (point - entry.point) * ((normal + entry.normal) / 2);
		return (d >= -0.01 && weight(entry, point, normal) > 1 / threshold);
	}

	bool IrradianceCache::testOctreeNode(OctreeNode *node, const Math::Point &origin, float size, const Math::Point &point, const Math::Normal &normal) const
	{
		if (!node) {
			return false;
		}

		for (const Entry &e : node->entries)
		{
			if (isEntryValid(e, point, normal, mThreshold)) {
				return true;
			}
		}

		for (int i = 0; i < 8; i++) {
			Math::Point childOrigin;
			float childSize;

			getChildNode(origin, size, i, childOrigin, childSize);

			float distance2 = distance2ToNode(point, i, childOrigin, childSize);
			if (distance2 < childSize * childSize) {
				if (testOctreeNode(node->children[i].get(), childOrigin, childSize, point, normal)) {
					return true;
				}
			}
		}

		return false;
	}

	void IrradianceCache::interpolateOctreeNode(OctreeNode *node, const Math::Point &origin, float size, const Math::Point &point, const Math::Normal &normal, float threshold, Object::Radiance &irradiance, float &totalWeight) const
	{
		if (!node) {
			return;
		}

		for (const Entry &entry : node->entries)
		{
			if (isEntryValid(entry, point, normal, threshold)) {
				float w = weight(entry, point, normal);
				if (std::isinf(w)) {
					irradiance = entry.radiance;
					totalWeight = -1;
					break;
				}
				Math::Vector cross = Math::Vector(normal % entry.normal);
				Math::Vector dist = point - entry.point;
				irradiance += (entry.radiance + entry.rotGrad * cross + entry.transGrad * dist) * w;
				totalWeight += w;
			}
		}

		for (int i = 0; i < 8; i++) {
			Math::Point childOrigin;
			float childSize;

			if (totalWeight < 0) {
				break;
			}

			getChildNode(origin, size, i, childOrigin, childSize);

			float distance2 = distance2ToNode(point, i, childOrigin, childSize);
			if (distance2 < childSize * childSize) {
				interpolateOctreeNode(node->children[i].get(), childOrigin, childSize, point, normal, threshold, irradiance, totalWeight);
			}
		}
	}

	bool IrradianceCache::test(const Math::Point &point, const Math::Normal &normal) const
	{
		std::lock_guard<std::mutex> guard(mMutex);
		return testUnlocked(point, normal);
	}

	bool IrradianceCache::testUnlocked(const Math::Point &point, const Math::Normal &normal) const
	{
		return testOctreeNode(mOctreeRoot.get(), mOctreeOrigin, mOctreeSize, point, normal);
	}

	Object::Radiance IrradianceCache::interpolate(const Math::Point &point, const Math::Normal &normal) const
	{
		std::lock_guard<std::mutex> guard(mMutex);
		return interpolateUnlocked(point, normal);
	}

	Object::Radiance IrradianceCache::interpolateUnlocked(const Math::Point &point, const Math::Normal &normal) const
	{
		float totalWeight = 0;
		Object::Radiance irradiance;
		float threshold = mThreshold;

		for(int i=0; i<3; i++) {
			interpolateOctreeNode(mOctreeRoot.get(), mOctreeOrigin, mOctreeSize, point, normal, threshold, irradiance, totalWeight);

			if (totalWeight > 0) {
				irradiance = irradiance / totalWeight;
				break;
			}
			else {
				threshold *= 2;
			}
		}

		return irradiance.clamp();
	}

	void IrradianceCache::add(const Entry &entry)
	{
		std::lock_guard<std::mutex> guard(mMutex);

		float R = entry.radius * mThreshold;

		if (!mOctreeRoot) {
			mOctreeRoot = std::make_unique<OctreeNode>();
			mOctreeSize = R;
			mOctreeOrigin = entry.point;
		}

		while (std::abs(entry.point.x() - mOctreeOrigin.x()) > mOctreeSize ||
			   std::abs(entry.point.y() - mOctreeOrigin.y()) > mOctreeSize ||
			   std::abs(entry.point.z() - mOctreeOrigin.z()) > mOctreeSize ||
			   mOctreeSize < R) {
			float x = (entry.point.x() > mOctreeOrigin.x()) ? 1 : -1;
			float y = (entry.point.y() > mOctreeOrigin.y()) ? 1 : -1;
			float z = (entry.point.z() > mOctreeOrigin.z()) ? 1 : -1;

			int idx = ((x < 0) ? 1 : 0) + ((y < 0) ? 2 : 0) + ((z < 0) ? 4 : 0);
			std::unique_ptr<OctreeNode> newRoot = std::make_unique<OctreeNode>();
			newRoot->children[idx] = std::move(mOctreeRoot);
			mOctreeRoot = std::move(newRoot);
			mOctreeOrigin = mOctreeOrigin + Math::Vector(x, y, z) * mOctreeSize;
			mOctreeSize *= 2;
		}

		Math::Point origin = mOctreeOrigin;
		float size = mOctreeSize;
		OctreeNode *node = mOctreeRoot.get();
		while (size > R * 2) {
			float x = (entry.point.x() > origin.x()) ? 1 : -1;
			float y = (entry.point.y() > origin.y()) ? 1 : -1;
			float z = (entry.point.z() > origin.z()) ? 1 : -1;

			int idx = ((x > 0) ? 1 : 0) + ((y > 0) ? 2 : 0) + ((z > 0) ? 4 : 0);
			Math::Point newOrigin = origin + Math::Vector(x, y, z) * size / 2;

			if (!node->children[idx])
			{
				node->children[idx] = std::make_unique<OctreeNode>();
			}

			origin = newOrigin;
			size /= 2;
			node = node->children[idx].get();
		}

		node->entries.push_back(entry);
	}

	void IrradianceCache::clear()
	{
		mOctreeRoot.release();
	}
}
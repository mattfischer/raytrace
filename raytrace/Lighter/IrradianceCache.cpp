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
	return 1.0f / ((point - entry.point).magnitude() / entry.radius + std::sqrt(1 - normal * entry.normal));
}

float IrradianceCache::error(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const
{
	return ((4.0f / M_PI) * (point - entry.point).magnitude() / entry.radius + std::sqrt(1 - normal * entry.normal));
}

void IrradianceCache::lookupOctreeNode(OctreeNode *node, Math::Point origin, float size, const Math::Point &point, const Math::Normal &normal, std::vector<Entry> &entries) const
{
	if (!node) {
		return;
	}

	for (const Entry &e : node->entries)
	{
		float d = (point - e.point) * ((normal + e.normal) / 2);
		if (d >= -0.01 && weight(e, point, normal) > 1 / mThreshold)
		{
			entries.push_back(e);
		}
	}

	for (int i = 0; i < 8; i++) {
		float x = (i & 1) ? 1 : -1;
		float y = (i & 2) ? 1 : -1;
		float z = (i & 4) ? 1 : -1;

		float childSize = size / 2;
		Math::Point childOrigin = origin + Math::Vector(x, y, z) * childSize;

		float distance = 0;
		float d;
		if (point.x() < childOrigin.x() - childSize) { d = ((childOrigin.x() - childSize) - point.x()); distance += d * d; }
		if (point.y() < childOrigin.y() - childSize) { d = ((childOrigin.y() - childSize) - point.y()); distance += d * d; }
		if (point.z() < childOrigin.z() - childSize) { d = ((childOrigin.z() - childSize) - point.z()); distance += d * d; }
		if (point.x() > childOrigin.x() + childSize) { d = (point.x() - (childOrigin.x() + childSize)); distance += d * d; }
		if (point.y() > childOrigin.y() + childSize) { d = (point.y() - (childOrigin.y() + childSize)); distance += d * d; }
		if (point.z() > childOrigin.z() + childSize) { d = (point.z() - (childOrigin.z() + childSize)); distance += d * d; }
		distance = std::sqrt(distance);

		if (distance < childSize) {
			lookupOctreeNode(node->children[i].get(), childOrigin, childSize, point, normal, entries);
		}
	}
}

std::vector<IrradianceCache::Entry> IrradianceCache::lookup(const Math::Point &point, const Math::Normal &normal) const
{
	std::lock_guard<std::mutex> guard(mMutex);
	return lookupUnlocked(point, normal);
}

std::vector<IrradianceCache::Entry> IrradianceCache::lookupUnlocked(const Math::Point &point, const Math::Normal &normal) const
{
	std::vector<Entry> entries;

	lookupOctreeNode(mOctreeRoot.get(), mOctreeOrigin, mOctreeSize, point, normal, entries);

	return entries;
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
#define _USE_MATH_DEFINES
#include "Lighter/IrradianceCache.hpp"

#include "Math/Vector.hpp"

#include <algorithm>
#include <cmath>

namespace Lighter {
    IrradianceCache::RadianceGradient::RadianceGradient(const Object::Radiance &radiance, const Math::Vector &vector)
    {
        mRed = vector * radiance.red();
        mGreen = vector * radiance.green();
        mBlue = vector * radiance.blue();
    }

    IrradianceCache::RadianceGradient::RadianceGradient(const Math::Vector &red, const Math::Vector &green, const Math::Vector &blue)
    {
        mRed = red;
        mGreen = green;
        mBlue = blue;
    }

    Object::Radiance IrradianceCache::RadianceGradient::operator*(const Math::Vector &vector) const
    {
        return Object::Radiance(mRed * vector, mGreen * vector, mBlue * vector);
    }

    IrradianceCache::RadianceGradient IrradianceCache::RadianceGradient::operator+(const RadianceGradient &other) const
    {
        return RadianceGradient(mRed + other.mRed, mGreen + other.mGreen, mBlue + other.mBlue);
    }

    IrradianceCache::RadianceGradient &IrradianceCache::RadianceGradient::operator+=(const RadianceGradient &other)
    {
        mRed = mRed + other.mRed;
        mGreen = mGreen + other.mGreen;
        mBlue = mBlue + other.mBlue;

        return *this;
    }

    IrradianceCache::RadianceGradient IrradianceCache::RadianceGradient::operator*(float other) const
    {
        return RadianceGradient(mRed * other, mGreen * other, mBlue * other);
    }

    IrradianceCache::RadianceGradient IrradianceCache::RadianceGradient::operator/(float other) const
    {
        return RadianceGradient(mRed / other, mGreen / other, mBlue / other);
    }

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
        float x = (idx & 1) ? 1.0f : -1.0f;
        float y = (idx & 2) ? 1.0f : -1.0f;
        float z = (idx & 4) ? 1.0f : -1.0f;

        childSize = size / 2;
        childOrigin = origin + Math::Vector(x, y, z) * childSize;
    }

    bool IrradianceCache::isEntryValid(const Entry &entry, const Math::Point &point, const Math::Normal &normal, float weight, float threshold) const
    {
        float d = (point - entry.point) * ((normal + entry.normal) / 2);
        return (d >= -0.01 && weight > 1 / threshold);
    }

    bool IrradianceCache::visitOctreeNode(OctreeNode *node, const Math::Point &origin, float size, const Math::Point &point, const std::function<bool(const Entry &)> &callback) const
    {
        if (!node) {
            return true;
        }

        for (const Entry &entry : node->entries)
        {
            if (!callback(entry)) {
                return false;
            }
        }

        for (int i = 0; i < 8; i++) {
            Math::Point childOrigin;
            float childSize;

            getChildNode(origin, size, i, childOrigin, childSize);

            float distance2 = distance2ToNode(point, i, childOrigin, childSize);
            if (distance2 < childSize * childSize) {
                if (!visitOctreeNode(node->children[i].get(), childOrigin, childSize, point, callback)) {
                    return false;
                }
            }
        }

        return true;
    }

    bool IrradianceCache::test(const Math::Point &point, const Math::Normal &normal) const
    {
        std::lock_guard<std::mutex> guard(mMutex);
        return testUnlocked(point, normal);
    }

    bool IrradianceCache::testUnlocked(const Math::Point &point, const Math::Normal &normal) const
    {
        bool ret = false;
        auto callback = [&](const Entry &entry) {
            float w = weight(entry, point, normal);
            if (isEntryValid(entry, point, normal, w, mThreshold)) {
                ret = true;
                return false;
            }
            return true;
        };

        visitOctreeNode(mOctreeRoot.get(), mOctreeOrigin, mOctreeSize, point, std::ref(callback));

        return ret;
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

        auto callback = [&] (const Entry &entry) {
            float w = weight(entry, point, normal);
            if (isEntryValid(entry, point, normal, w, threshold)) {
                if (std::isinf(w)) {
                    irradiance = entry.radiance;
                    totalWeight = 1;
                    return false;
                }
                Math::Vector cross = Math::Vector(normal % entry.normal);
                Math::Vector dist = point - entry.point;
                irradiance += (entry.radiance + entry.rotGrad * cross + entry.transGrad * dist) * w;
                totalWeight += w;
            }
            return true;
        };

        for(int i=0; i<3; i++) {
            visitOctreeNode(mOctreeRoot.get(), mOctreeOrigin, mOctreeSize, point, std::ref(callback));

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
            float x = (entry.point.x() > mOctreeOrigin.x()) ? 1.0f : -1.0f;
            float y = (entry.point.y() > mOctreeOrigin.y()) ? 1.0f : -1.0f;
            float z = (entry.point.z() > mOctreeOrigin.z()) ? 1.0f : -1.0f;

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
            float x = (entry.point.x() > origin.x()) ? 1.0f : -1.0f;
            float y = (entry.point.y() > origin.y()) ? 1.0f : -1.0f;
            float z = (entry.point.z() > origin.z()) ? 1.0f : -1.0f;

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

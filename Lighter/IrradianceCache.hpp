#ifndef LIGHTER_IRRADIANCE_CACHE_HPP
#define LIGHTER_IRRADIANCE_CACHE_HPP

#include "Math/Point.hpp"
#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Object/Radiance.hpp"

#include <vector>
#include <mutex>
#include <functional>

namespace Lighter {
    class IrradianceCache
    {
    public:
        class RadianceGradient
        {
        public:
            RadianceGradient() = default;
            RadianceGradient(const Object::Radiance &radiance, const Math::Vector &vector);
            RadianceGradient(const Math::Vector &red, const Math::Vector &green, const Math::Vector &blue);

            Object::Radiance operator*(const Math::Vector &vector) const;
            RadianceGradient operator+(const RadianceGradient &other) const;
            RadianceGradient& operator+=(const RadianceGradient &other);
            RadianceGradient operator*(float other) const;
            RadianceGradient operator/(float other) const;

        private:
            Math::Vector mRed;
            Math::Vector mGreen;
            Math::Vector mBlue;
        };

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

        bool visitOctreeNode(OctreeNode *node, const Math::Point &origin, float size, const Math::Point &point, const std::function<bool(const Entry &)> &callback) const;

        float distance2ToNode(const Math::Point &point, int idx, const Math::Point &origin, float size) const;
        void getChildNode(const Math::Point &origin, float size, int idx, Math::Point &childOrigin, float &childSize) const;
        bool isEntryValid(const Entry &entry, const Math::Point &point, const Math::Normal &normal, float weight, float threshold) const;

        std::unique_ptr<OctreeNode> mOctreeRoot;
        Math::Point mOctreeOrigin;
        float mOctreeSize;
        mutable std::mutex mMutex;
        float mThreshold;
    };
}

#endif

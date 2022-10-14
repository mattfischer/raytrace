#include "BoundingVolume.hpp"

#include <algorithm>
#include <cmath>

namespace Object {
    const float root33 = std::sqrt(3) / 3.0f;

    std::array<Math::Vector, BoundingVolume::NUM_VECTORS> sVectors{ Math::Vector(1, 0, 0), Math::Vector(0, 1, 0), Math::Vector(0, 0, 1),
                                                                    /*Math::Vector(root33, root33, root33), Math::Vector(-root33, root33, root33),
                                                                    Math::Vector(root33, -root33, root33), Math::Vector(-root33, -root33, root33)*/ };

    BoundingVolume::BoundingVolume()
    {
        for (int i = 0; i < NUM_VECTORS; i++) {
            mMins[i] = FLT_MAX;
            mMaxes[i] = -FLT_MAX;
        }
    }

    BoundingVolume::BoundingVolume(const float mins[NUM_VECTORS], const float maxes[NUM_VECTORS])
    {
        for (int i = 0; i < NUM_VECTORS; i++) {
            mMins[i] = mins[i];
            mMaxes[i] = maxes[i];
        }
    }

    void BoundingVolume::expand(const Math::Point &point)
    {
        for (int i = 0; i < NUM_VECTORS; i++) {
            float dist = Math::Vector(point) * vectors()[i];
            if (dist < mMins[i]) {
                mMins[i] = dist;
            }
            if (dist > mMaxes[i]) {
                mMaxes[i] = dist;
            }
        }
    }

    void BoundingVolume::expand(const BoundingVolume &volume)
    {
        for (int i = 0; i < NUM_VECTORS; i++) {
            mMins[i] = std::min(volume.mMins[i], mMins[i]);
            mMaxes[i] = std::max(volume.mMaxes[i], mMaxes[i]);
        }
    }

    const std::array<Math::Vector, BoundingVolume::NUM_VECTORS> &BoundingVolume::vectors()
    {
        return sVectors;
    }

    bool BoundingVolume::intersectRay(const RayData &rayData, float &distance) const
    {
        float maxDistance;
        return intersectRay(rayData, distance, maxDistance);
    }

    bool BoundingVolume::intersectRay(const RayData &rayData, float &minDistance, float &maxDistance) const
    {
        float currentMinDist = -FLT_MAX;
        float currentMaxDist = FLT_MAX;

        for (int i = 0; i < NUM_VECTORS; i++) {
            float offset = rayData.offsets[i];
            float dot = rayData.dots[i];

            float min;
            float max;

            if (dot < 0) {
                max = (mMins[i] - offset) / dot;
                min = (mMaxes[i] - offset) / dot;
            }
            else if (dot == 0) {
                if (offset > mMaxes[i] || offset < mMins[i]) {
                    return false;
                }
                else {
                    continue;
                }
            }
            else {
                min = (mMins[i] - offset) / dot;
                max = (mMaxes[i] - offset) / dot;
            }

            currentMinDist = std::max(currentMinDist, min);
            currentMaxDist = std::min(currentMaxDist, max);

            if (currentMinDist > currentMaxDist || currentMaxDist < 0) {
                return false;
            }
        }

        if (currentMinDist > currentMaxDist || currentMaxDist < 0) {
            return false;
        }

        minDistance = currentMinDist;
        maxDistance = currentMaxDist;
        return true;
    }

    BoundingVolume::RayData BoundingVolume::getRayData(const Math::Ray &ray)
    {
        RayData rayData;

        for (int i = 0; i < NUM_VECTORS; i++) {
            const Math::Vector &vector = sVectors[i];
            rayData.offsets[i] = Math::Vector(ray.origin()) * vector;
            rayData.dots[i] = ray.direction() * vector;
        }

        return rayData;
    }

    Math::Point BoundingVolume::centroid() const
    {
        float d0 = (mMins[0] + mMaxes[0]) / 2;
        float d1 = (mMins[1] + mMaxes[1]) / 2;
        float d2 = (mMins[2] + mMaxes[2]) / 2;

        const Math::Vector &v0 = sVectors[0];
        const Math::Vector &v1 = sVectors[1];
        const Math::Vector &v2 = sVectors[2];

        float d = v0 * (v1 % v2);

        return Math::Point((v1 % v2) * (d0 / d) + (v2 % v0) * (d1 / d) + (v0 % v1) * (d2 / d));
    }
}

#ifndef OBJECT_BOUNDING_VOLUME_HPP
#define OBJECT_BOUNDING_VOLUME_HPP

#include "Math/Vector.hpp"
#include "Math/Ray.hpp"

#include <array>

namespace Object {
    class BoundingVolume
    {
    public:
        static const int NUM_VECTORS = 3;
        struct RayData {
            float offsets[NUM_VECTORS];
            float dots[NUM_VECTORS];
        };

        BoundingVolume();
        BoundingVolume(const float mins[NUM_VECTORS], const float maxes[NUM_VECTORS]);

        bool intersectRay(const RayData &rayData, float &distance) const;
        bool intersectRay(const RayData &rayData, float &minDistance, float &maxDistance) const;
        void expand(const Math::Point &point);
        void expand(const BoundingVolume &volume);

        Math::Point centroid() const;

        static const std::array<Math::Vector, NUM_VECTORS> &vectors();
        static RayData getRayData(const Math::Ray &ray);

    private:
        float mMins[NUM_VECTORS];
        float mMaxes[NUM_VECTORS];
    };
}

#endif

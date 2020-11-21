#ifndef OBJECT_SHAPE_BEZIER_PATCH_HPP
#define OBJECT_SHAPE_BEZIER_PATCH_HPP

#include "Object/Shape/Base.hpp"
#include "Object/Shape/Grid.hpp"

#include <vector>

namespace Object {
    namespace Shape {
        class BezierPatch : public Base {
        public:
            BezierPatch(int width, int height, std::vector<Math::Point> &&controlPoints);

            virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const;
            virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;

        private:
            std::unique_ptr<Grid> mGrid;
        };
    }
}

#endif
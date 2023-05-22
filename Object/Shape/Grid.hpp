#ifndef OBJECT_SHAPE_GRID_HPP
#define OBJECT_SHAPE_GRID_HPP

#include "Object/Shape/Base.hpp"
#include "Object/BoundingVolumeHierarchy.hpp"

#include "Math/Point.hpp"
#include "Math/Bivector.hpp"

#include <vector>
#include <functional>

namespace Object {
    namespace Shape {
        class Grid : public Base
        {
        public:
            struct Vertex {
                Math::Point point;
                Math::Normal normal;
                Math::Bivector tangent;
            };

            Grid(unsigned int width, unsigned int height, std::vector<Vertex> &&vertices);

            bool intersect(const Math::Ray &ray, Intersection &isect) const override;
            BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

        private:
            unsigned int computeBounds(std::vector<Object::BoundingVolumeHierarchy::Node> &nodes, unsigned int u, unsigned int v, unsigned int du, unsigned int dv) const;
            const Vertex &vertex(unsigned int u, unsigned int v) const;

            unsigned int mWidth;
            unsigned int mHeight;
            std::vector<Vertex> mVertices;
            Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
        };
    }
}
#endif

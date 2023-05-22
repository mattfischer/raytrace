#ifndef OBJECT_SHAPE_GROUP_HPP
#define OBJECT_SHAPE_GROUP_HPP

#include "Object/Shape/Base.hpp"

namespace Object {
    namespace Shape {
        class Group : public Base
        {
        public:
            Group(std::vector<std::unique_ptr<Base>> &&shapes);

            bool intersect(const Math::Ray &ray, Intersection &isect) const override;
            BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

        private:
            std::vector<std::unique_ptr<Base>> mShapes;
            std::vector<BoundingVolume> mVolumes;
        };
    }
}

#endif
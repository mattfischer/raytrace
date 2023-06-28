#ifndef OBJECT_SHAPE_SPHERE_HPP
#define OBJECT_SHAPE_SPHERE_HPP

#include "Object/Shape/Base.hpp"

namespace Object {
    namespace Shape {
        class Sphere : public Base
        {
        public:
            Sphere(const Math::Point &position, float radius);

            bool intersect(const Math::Ray &ray, Intersection &isect) const override;
            BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

            void writeProxy(ShapeProxy &proxy) const override;

        private:
            Math::Point mPosition;
            float mRadius;
        };
    }
}

#endif

#ifndef OBJECT_IMPL_SHAPE_TRIANGLE_HPP
#define OBJECT_IMPL_SHAPE_TRIANGLE_HPP

#include "Math/Ray.hpp"
#include "Math/Point.hpp"

namespace Object::Impl::Shape {
    class Triangle
    {
    public:
        static bool intersect(const Math::Ray &ray, const Math::Point &p, const Math::Point &pu, const Math::Point &pv, float &distance, float &u, float &v);
    };
}
#endif
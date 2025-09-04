#ifndef OBJECT_IMPL_SHAPE_GROUP_HPP
#define OBJECT_IMPL_SHAPE_GROUP_HPP

#include "Object/Shape.hpp"

#include "Object/Impl/Shape/CLProxies.hpp"

namespace Object::Impl::Shape {
    class Group : public Object::Shape
    {
    public:
        Group(std::vector<std::unique_ptr<Object::Shape>> shapes);

        bool intersect(const Math::Ray &ray, Intersection &isect, bool closest) const override;
        Object::BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

        void writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const override;

    private:
        std::vector<std::unique_ptr<Object::Shape>> mShapes;
        std::vector<Object::BoundingVolume> mVolumes;
    };
}

#endif
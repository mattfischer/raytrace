#ifndef OBJECT_IMPL_SHAPE_QUAD_HPP
#define OBJECT_IMPL_SHAPE_QUAD_HPP

#include "Object/Shape.hpp"

#include "Math/Bivector.hpp"

namespace Object::Impl::Shape {
    class Quad : public Object::Shape
    {
    public:
        Quad(const Math::Point &position, const Math::Vector &side1, const Math::Vector &side2);

        bool intersect(const Math::Ray &ray, Intersection &isect, bool closest) const override;
        BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

        bool sample(Math::Sampler &sampler, Math::Point &pnt, Math::Normal &nrm, float &pdf) const override;
        float samplePdf(const Math::Point &pnt) const override;

        void writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const override;

    private:
        Math::Point mPosition;
        Math::Vector mSide1;
        Math::Vector mSide2;
        Math::Normal mNormal;
        Math::Bivector mTangent;
    };
}

#endif

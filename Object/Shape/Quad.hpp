#ifndef OBJECT_SHAPE_QUAD_HPP
#define OBJECT_SHAPE_QUAD_HPP

#include "Object/Shape/Base.hpp"

#include "Math/Bivector.hpp"

namespace Object {
    namespace Shape {
        class Quad : public Base, public Base::Sampler
        {
        public:
            Quad(const Math::Point &position, const Math::Vector &side1, const Math::Vector &side2);

            virtual bool intersect(const Math::Ray &ray, Intersection &isect) const;
            virtual BoundingVolume boundingVolume(const Math::Transformation &trans) const;

            virtual const Base::Sampler *sampler() const;
            float surfaceArea() const;
            virtual bool sample(const Math::Point2D &pntSurface, Math::Point &pnt, Math::Normal &nrm) const;

        private:
            Math::Point mPosition;
            Math::Vector mSide1;
            Math::Vector mSide2;
            Math::Normal mNormal;
            Math::Bivector mTangent;
        };
    }
}

#endif

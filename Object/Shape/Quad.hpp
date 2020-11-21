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

            virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const;
            virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;

            virtual const Sampler *sampler() const;
            float surfaceArea() const;
            virtual bool sample(const Math::Point2D &surfacePoint, Math::Point &point, Math::Normal &normal) const;

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
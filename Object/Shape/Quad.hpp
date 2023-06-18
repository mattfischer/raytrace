#ifndef OBJECT_SHAPE_QUAD_HPP
#define OBJECT_SHAPE_QUAD_HPP

#include "Object/Shape/Base.hpp"

#include "Math/Bivector.hpp"

namespace Object {
    namespace Shape {
        class Quad : public Base
        {
        public:
            Quad(const Math::Point &position, const Math::Vector &side1, const Math::Vector &side2);

            bool intersect(const Math::Ray &ray, Intersection &isect) const override;
            BoundingVolume boundingVolume(const Math::Transformation &trans) const override;

            bool sample(Math::Sampler::Base &sampler, Math::Point &pnt, Math::Normal &nrm, float &pdf) const override;
            float samplePdf(const Math::Point &pnt) const override;

            void writeProxy(PrimitiveProxy &proxy) const override;

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

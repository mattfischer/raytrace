#ifndef OBJECT_SHAPE_BASE_HPP
#define OBJECT_SHAPE_BASE_HPP

#include "Math/Ray.hpp"
#include "Math/Normal.hpp"
#include "Math/Point2D.hpp"
#include "Math/Bivector.hpp"
#include "Math/Transformation.hpp"
#include "Math/Sampler/Base.hpp"

#include "Object/BoundingVolume.hpp"

#include <vector>
#include <memory>

namespace Object {
    namespace Shape {
        class Base
        {
        public:
            struct Intersection {
                float distance;
                Math::Normal normal;
                Math::Bivector tangent;
                Math::Point2D surfacePoint;
            };

            class Sampler {
            public:
                virtual float surfaceArea() const = 0;
                virtual bool sample(Math::Sampler::Base &sampler, Math::Point &pnt, Math::Normal &nrm) const = 0;
            };

            virtual bool intersect(const Math::Ray &ray, Intersection &isect) const = 0;
            virtual BoundingVolume boundingVolume(const Math::Transformation &trans) const = 0;
            virtual const Sampler *sampler() const { return nullptr; }
        };
    }
}

#endif
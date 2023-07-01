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

#include "Proxies.hpp"

namespace Object {
    namespace Shape {
        class Base
        {
        public:
            struct Intersection {
                Intersection(const ShapeIntersectionProxy &proxy) {
                    distance = proxy.distance;
                    normal = Math::Normal(proxy.normal);
                }

                Intersection() = default;

                float distance;
                Math::Normal normal;
                Math::Bivector tangent;
                Math::Point2D surfacePoint;

                void writeProxy(ShapeIntersectionProxy &proxy) const
                {
                    proxy.distance = distance;
                    normal.writeProxy(proxy.normal);
                }
            };

            virtual bool intersect(const Math::Ray &ray, Intersection &isect) const = 0;
            virtual BoundingVolume boundingVolume(const Math::Transformation &trans) const = 0;
            virtual bool sample(Math::Sampler::Base &sampler, Math::Point &pnt, Math::Normal &nrm, float &pdf) const { return false; }
            virtual float samplePdf(const Math::Point &pnt) const { return 0; }

            virtual void writeProxy(ShapeProxy &proxy) const { proxy.type = ShapeProxy::Type::None; }
        };
    }
}

#endif
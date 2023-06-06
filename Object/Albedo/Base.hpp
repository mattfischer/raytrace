#ifndef OBJECT_ALBEDO_BASE_HPP
#define OBJECT_ALBEDO_BASE_HPP

#include "Object/Color.hpp"

#include "Math/Point2D.hpp"
#include "Math/Bivector2D.hpp"

namespace Object {
    namespace Albedo {
        class Base
        {
        public:
            virtual Object::Color color(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection) const = 0;
            virtual bool needSurfaceProjection() const = 0;
        };
    }
}

#endif
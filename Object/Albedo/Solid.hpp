#ifndef OBJECT_ALBEDO_SOLID_HPP
#define OBJECT_ALBEDO_SOLID_HPP

#include "Object/Albedo/Base.hpp"

namespace Object {
    namespace Albedo {
        class Solid : public Base
        {
        public:
            Solid(const Object::Color &color);

            Object::Color color(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection) const override;

        protected:
            Object::Color mColor;
        };
    }
}
#endif
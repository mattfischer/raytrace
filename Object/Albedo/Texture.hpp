#ifndef OBJECT_ALBEDO_TEXTURE_HPP
#define OBJECT_ALBEDO_TEXTURE_HPP

#include "Object/Albedo/Base.hpp"

#include "Object/Texture.hpp"

#include <memory>

namespace Object {
    namespace Albedo {
        class Texture : public Base
        {
        public:
            Texture(std::unique_ptr<Object::Texture<3>> texture);

            virtual Object::Color color(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection) const;

        protected:
            std::unique_ptr<Object::Texture<3>> mTexture;
        };
    }
}

#endif
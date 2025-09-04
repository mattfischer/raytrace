#ifndef OBJECT_IMPL_ALBEDO_TEXTURE_HPP
#define OBJECT_IMPL_ALBEDO_TEXTURE_HPP

#include "Object/Albedo.hpp"

#include "Object/Texture.hpp"

#include <memory>

namespace Object::Impl::Albedo {
    class Texture : public Object::Albedo
    {
    public:
        Texture(std::unique_ptr<Object::Texture<3>> texture);

        Math::Color color(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection) const override;
        bool needSurfaceProjection() const override;

        void writeProxy(AlbedoProxy &proxy, OpenCL::Allocator &clAllocator) const override;

    protected:
        std::unique_ptr<Object::Texture<3>> mTexture;
    };
}

#endif
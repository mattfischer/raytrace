#include "Object/Albedo/Texture.hpp"

#include <fstream>

#include <windows.h>

namespace Object::Albedo {
    Texture::Texture(std::unique_ptr<Object::Texture<3>> texture)
        : mTexture(std::move(texture))
    {
        mTexture->generateMipMaps();
    }

    Math::Color Texture::color(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection) const
    {
        Object::Texture<3>::Value value = mTexture->sample(surfacePoint, surfaceProjection);

        return Math::Color(value.channels[0], value.channels[1], value.channels[2]);
    }

    bool Texture::needSurfaceProjection() const
    {
        return true;
    }

    void Texture::writeProxy(AlbedoProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        proxy.type = AlbedoProxy::Type::Texture;
        mTexture->writeProxy(proxy.texture.texture, clAllocator);
    }       
}
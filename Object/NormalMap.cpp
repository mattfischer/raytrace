#include "Object/NormalMap.hpp"

namespace Object {
    NormalMap::NormalMap(std::unique_ptr<Object::Texture<3>> texture, float magnitude)
    {
        const TextureBase::MipMap &map = texture->mipMap(0);
        std::vector<float> values(map.width() * map.height() * 2);
        for (int j = 0; j < map.height(); j++) {
            for (int i = 0; i < map.width(); i++) {
                float s = 0;
                float su = 0;
                float sv = 0;
                for (int c = 0; c < 3; c++) {
                    s += map.at(i, j, c) / 3.0f;
                    su += map.at((i == map.width() - 1) ? 0 : i + 1, j, c) / 3.0f;
                    sv += map.at(i, (j == map.height() - 1) ? 0 : j + 1, c) / 3.0f;
                }
                values[(j * map.width() + i) * 2 + 0] = -(su - s) * map.width() * magnitude;
                values[(j * map.width() + i) * 2 + 1] = -(sv - s) * map.height() * magnitude;
            }
        }

        mTexture = std::make_unique<Object::Texture<2>>(map.width(), map.height(), std::move(values));
        mTexture->generateMipMaps();
    }

    Math::Normal NormalMap::perturbNormal(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection, const Math::Normal &normal, const Math::Bivector &tangent) const
    {
        Texture<2>::Value value = mTexture->sample(surfacePoint, surfaceProjection);
        Math::Vector offset = tangent * Math::Vector2D(value.channels[0], value.channels[1]);
        return (normal + Math::Normal(offset)).normalize();
    }

    void NormalMap::writeProxy(NormalMapProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        mTexture->writeProxy(proxy.texture, clAllocator);
    }
}
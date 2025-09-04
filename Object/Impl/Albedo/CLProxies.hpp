#ifndef OBJECT_ALBEDO_CLPROXIES_HPP
#define OBJECT_ALBEDO_CLPROXIES_HPP

#include "Math/CLProxies.hpp"

struct MipMapProxy {
    int width;
    int height;
    int numChannels;
    float *values;
};

struct TextureProxy {
    int numMipMaps;
    MipMapProxy *mipMaps;
};

struct AlbedoSolidProxy {
    ColorProxy color;
};

struct AlbedoTextureProxy {
    TextureProxy texture;
};

struct AlbedoProxy {
    enum Type {
        Solid,
        Texture
    };

    Type type;
    union {
        AlbedoSolidProxy solid;
        AlbedoTextureProxy texture;
    };
};

#endif
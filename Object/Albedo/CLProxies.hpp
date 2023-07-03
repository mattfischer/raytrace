#ifndef OBJECT_ALBEDO_CLPROXIES_HPP
#define OBJECT_ALBEDO_CLPROXIES_HPP

#include "Math/CLProxies.hpp"

struct AlbedoSolidProxy {
    ColorProxy color;
};

struct AlbedoProxy {
    enum Type {
        Solid,
        Texture
    };

    Type type;
    union {
        AlbedoSolidProxy solid;
    };
};

#endif
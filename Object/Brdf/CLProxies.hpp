#ifndef OBJECT_BRDF_CLPROXIES_HPP
#define OBJECT_BRDF_CLPROXIES_HPP

struct BrdfLambertProxy {
    float strength;
};

struct BrdfOrenNayarProxy {
    float strength;
    float roughness;
};

struct BrdfPhongProxy {
    float strength;
    float power;
};

struct BrdfTorranceSparrowProxy {
    float strength;
    float roughness;
    float ior;
};

struct BrdfProxy {
    enum Type {
        Lambert,
        OrenNayar,
        Phong,
        TorranceSparrow
    };

    Type type;
    union {
        BrdfLambertProxy lambert;
        BrdfOrenNayarProxy orenNayar;
        BrdfPhongProxy phong;
        BrdfTorranceSparrowProxy torranceSparrow;
    };
};

#endif
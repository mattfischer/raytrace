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

struct BrdfProxy {
    enum Type {
        Lambert,
        OrenNayar,
        Phong
    };

    Type type;
    union {
        BrdfLambertProxy lambert;
        BrdfOrenNayarProxy orenNayar;
        BrdfPhongProxy phong;
    };
};

#endif
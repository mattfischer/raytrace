#ifndef OBJECT_BRDF_CLPROXIES_HPP
#define OBJECT_BRDF_CLPROXIES_HPP

struct BrdfLambertProxy {
    float strength;
};

struct BrdfOrenNayarProxy {
    float strength;
    float roughness;
};

struct BrdfProxy {
    enum Type {
        Lambert,
        OrenNayar
    };

    Type type;
    union {
        BrdfLambertProxy lambert;
        BrdfOrenNayarProxy orenNayar;
    };
};

#endif
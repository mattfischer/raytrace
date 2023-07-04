#ifndef OBJECT_BRDF_CLPROXIES_HPP
#define OBJECT_BRDF_CLPROXIES_HPP

struct BrdfLambertProxy {
    float strength;
};

struct BrdfProxy {
    enum Type {
        Lambert
    };

    Type type;
    union {
        BrdfLambertProxy lambert;
    };
};

#endif
#ifndef OBJECT_SHAPE_CLPROXIES_HPP
#define OBJECT_SHAPE_CLPROXIES_HPP

#include "Math/CLProxies.hpp"

struct QuadShapeProxy {
    PointProxy position;
    VectorProxy side1;
    VectorProxy side2;
    NormalProxy normal;
};

struct SphereShapeProxy {
    PointProxy position;
    float radius;
};

struct ShapeProxy {
    enum Type {
        None,
        Quad,
        Sphere
    };

    Type type;
    union {
        struct QuadShapeProxy quad;
        struct SphereShapeProxy sphere;
    };
};

#endif
#ifndef OBJECT_SHAPE_CLPROXIES_HPP
#define OBJECT_SHAPE_CLPROXIES_HPP

#include "Math/CLProxies.hpp"

struct ShapeQuadProxy {
    PointProxy position;
    VectorProxy side1;
    VectorProxy side2;
    NormalProxy normal;
};

struct ShapeSphereProxy {
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
    TransformationProxy *transformation;
    union {
        struct ShapeQuadProxy quad;
        struct ShapeSphereProxy sphere;
    };
};

#endif
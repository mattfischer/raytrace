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

struct ShapeProxy;
struct ShapeTransformedProxy {
    TransformationProxy transformation;
    ShapeProxy *shape;
};

struct ShapeProxy {
    enum Type {
        None,
        Quad,
        Sphere,
        Transformed
    };

    Type type;
    union {
        struct ShapeQuadProxy quad;
        struct ShapeSphereProxy sphere;
        struct ShapeTransformedProxy transformed;
    };
};

#endif
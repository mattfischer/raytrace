#ifndef PROXIES_HPP
#define PROXIES_HPP

#include <stdint.h>

__declspec(align(16)) struct PointProxy {
    float coords[3];
};

__declspec(align(16)) struct VectorProxy {
    float coords[3];
};

__declspec(align(16)) struct NormalProxy {
    float coords[3];
};

struct QuadPrimitiveProxy {
    PointProxy position;
    VectorProxy side1;
    VectorProxy side2;
    NormalProxy normal;
};

struct SpherePrimitiveProxy {
    PointProxy position;
    float radius;
};

struct PrimitiveProxy {
    enum Type {
        None,
        Quad,
        Sphere
    };

    Type type;
    uintptr_t primitive;
    union {
        QuadPrimitiveProxy quad;
        SpherePrimitiveProxy sphere;
    };
};

struct SceneProxy {
    int numPrimitives;
    PrimitiveProxy *primitives;
};

struct RayProxy {
    PointProxy origin;
    VectorProxy direction;
};

struct ShapeIntersectionProxy {
    float distance;
    NormalProxy normal;
    uintptr_t primitive;
};

struct ItemProxy {
    RayProxy ray;
    ShapeIntersectionProxy shapeIntersection;
    RayProxy shadowRay;
    ShapeIntersectionProxy shadowShapeIntersection;
};

#endif
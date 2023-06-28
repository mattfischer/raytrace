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

struct PrimitiveProxy {
    ShapeProxy shape;
    uintptr_t primitive;
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
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

__declspec(align(16)) struct RadianceProxy {
    float coords[3];
};

__declspec(align(16)) struct ColorProxy {
    float coords[3];
};

struct BivectorProxy {
    VectorProxy u;
    VectorProxy v;
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

struct SurfaceProxy {
    RadianceProxy radiance;
};

struct PrimitiveProxy {
    ShapeProxy shape;
    SurfaceProxy surface;
    uintptr_t primitive;
};

struct PointLightProxy {
    PointProxy position;
    RadianceProxy radiance;
};

struct CameraProxy {
    PointProxy position;
    VectorProxy direction;
    BivectorProxy imagePlane;
    float imageSize;
    float focalLength;
    float apertureSize;
};

struct SceneProxy {
    int numPrimitives;
    PrimitiveProxy *primitives;
    int numAreaLights;
    PrimitiveProxy **areaLights;
    int numPointLights;
    PointLightProxy *pointLights;
    RadianceProxy skyRadiance;
    CameraProxy camera;
};

struct RayProxy {
    PointProxy origin;
    VectorProxy direction;
};

struct BeamProxy {
    RayProxy ray;
    BivectorProxy originDifferential;
    BivectorProxy directionDifferential;
};

struct ShapeIntersectionProxy {
    float distance;
    NormalProxy normal;
};

struct IntersectionProxy {
    ShapeIntersectionProxy shapeIntersection;
    PrimitiveProxy *primitive;
    BeamProxy *beam;
    PointProxy point;
};

struct SettingsProxy {
    int width;
    int height;
};

struct ItemProxy {
    BeamProxy beam;
    IntersectionProxy isect;
    RayProxy shadowRay;
    IntersectionProxy shadowIsect;
    bool specularBounce;
    int generation;
    float pdf;
    ColorProxy throughput;
    RadianceProxy radiance;
    float random[4];
    int lightIndex;
    int currentPixel;
    int x;
    int y;
    float shadowD;
    float shadowDot;
};

#endif
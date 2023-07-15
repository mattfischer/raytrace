#ifndef OBJECT_CLPROXIES_HPP
#define OBJECT_CLPROXIES_HPP

#include "Math/CLProxies.hpp"
#include "Object/Albedo/CLProxies.hpp"
#include "Object/Shape/CLProxies.hpp"
#include "Object/Brdf/CLProxies.hpp"

struct NormalMapProxy {
    TextureProxy texture;
};

struct SurfaceProxy {
    RadianceProxy radiance;
    AlbedoProxy albedo;
    int numBrdfs;
    BrdfProxy *brdfs;
    bool opaque;
    float transmitIor;
    NormalMapProxy *normalMap;
};

struct PrimitiveProxy {
    ShapeProxy shape;
    SurfaceProxy surface;
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


struct ShapeIntersectionProxy {
    float distance;
    NormalProxy normal;
    BivectorProxy tangent;
    Point2DProxy surfacePoint;
};

struct IntersectionProxy {
    ShapeIntersectionProxy shapeIntersection;
    PrimitiveProxy *primitive;
    BeamProxy *beam;
    PointProxy point;
    NormalProxy normal;
    NormalProxy facingNormal;
    Bivector2DProxy surfaceProjection;
};

#endif
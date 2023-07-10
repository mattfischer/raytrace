#ifndef OBJECT_SHAPE_CLPROXIES_HPP
#define OBJECT_SHAPE_CLPROXIES_HPP

#include "Math/CLProxies.hpp"

struct BVHLimits {
    float values[3];
};

struct BoundingVolumeProxy {
    BVHLimits mins;
    BVHLimits maxes;
};

struct BVHNodeProxy {
    BoundingVolumeProxy volume;
    int index;
};

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

__declspec(align(16)) struct TriangleVertices {
    int values[3];
};

struct TriangleProxy {
    TriangleVertices vertices;
    NormalProxy normal;
};

struct ShapeTriangleMeshProxy {
    PointProxy *vertices;
    TriangleProxy *triangles;
    BVHNodeProxy *bvh;
};

struct GridVertexProxy {
    PointProxy point;
    NormalProxy normal;
};

struct GridProxy {
    int width;
    int height;
    GridVertexProxy *vertices;
    BVHNodeProxy *bvh;
};

struct ShapeGridsProxy {
    int numChildren;
    GridProxy *grids;
    BoundingVolumeProxy *volumes;
};

struct ShapeProxy {
    enum Type {
        None,
        Quad,
        Sphere,
        TriangleMesh,
        Grids
    };

    Type type;
    TransformationProxy *transformation;
    union {
        struct ShapeQuadProxy quad;
        struct ShapeSphereProxy sphere;
        struct ShapeTriangleMeshProxy triangleMesh;
        struct ShapeGridsProxy grids;
    };
};

#endif
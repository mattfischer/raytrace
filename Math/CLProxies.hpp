#ifndef MATH_CLPROXIES_HPP
#define MATH_CLPROXIES_HPP

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

__declspec(align(16)) struct MatrixProxy {
    float values[16];
};

__declspec(align(8)) struct Point2DProxy {
    float values[2];
};

__declspec(align(8)) struct Vector2DProxy {
    float values[2];
};

struct TransformationProxy {
    MatrixProxy matrix;
    MatrixProxy inverseMatrix;
};

struct BivectorProxy {
    VectorProxy u;
    VectorProxy v;
};

struct Bivector2DProxy {
    Vector2DProxy u;
    Vector2DProxy v;
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

#endif